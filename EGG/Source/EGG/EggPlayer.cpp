// Fill out your copyright notice in the Description page of Project Settings.


#include "EggPlayer.h"

#include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h" 
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
// Sets default values
AEggPlayer::AEggPlayer()
{

	// StaticMeshComponent��ǉ����ARootComponent�ɐݒ肷��
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = Sphere;

	// StaticMesh��Laod����StaticMeshComponent��StaticMesh�ɐݒ肷��
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere"));

	// StaticMesh��StaticMeshComponent�ɐݒ肷��
	Sphere->SetStaticMesh(Mesh);

	// Material��StaticMesh�ɐݒ肷��
	UMaterial* Material = LoadObject<UMaterial>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));

	// Material��StaticMeshComponent�ɐݒ肷��
	Sphere->SetMaterial(0, Material);

	// Simulate Physics��L���ɂ���
	Sphere->SetSimulatePhysics(true);

	// Hit Event��L���ɂ���
	Sphere->BodyInstance.bNotifyRigidBodyCollision = true;

	// SpringArm��ǉ�����
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);

	// Spring Arm�̒����𒲐�����
	SpringArm->TargetArmLength = 450.0f;

	// Pawn��ControllerRotation���g�p����
	SpringArm->bUsePawnControlRotation = true;

	// Camera��Lag��L���ɂ���
	SpringArm->bEnableCameraLag = true;

	// Camera��ǉ�����
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);

	// MotionBlur���I�t�ɂ���
	Camera->PostProcessSettings.MotionBlurAmount = 0.0f;

	// Input Mapping Context�uIM_Controls�v��ǂݍ���
	DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Input/PlayerInput"));

	// Input Action�uIA_Control�v��ǂݍ���
	ControlAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Control"));

	// Input Action�uIA_Look�v��ǂݍ���
	LookAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Look"));

	// Input Action�uIA_Jump�v��ǂݍ���
	JumpAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Jump"));

	// �f�t�H���g�l
	bIsGoalReached = false;

}

// Called when the game starts or when spawned
void AEggPlayer::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AEggPlayer::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	CanJump = true;
}

// Called to bind functionality to input
void AEggPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		// ControlBall��IA_Control��Triggered��Bind����
		EnhancedInputComponent->BindAction(ControlAction, ETriggerEvent::Triggered, this, &AEggPlayer::ControlBall);

		// Look��IA_Look��Triggered��Bind����
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEggPlayer::Look);

		// Jump��IA_Jump��Triggered��Bind����
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AEggPlayer::Jump);

	}
}

void AEggPlayer::OnGoalReached()
{
	if (bIsGoalReached) return; // ��d����h�~
	bIsGoalReached = true;

	// �������~�߂�
	Sphere->SetPhysicsLinearVelocity(FVector::ZeroVector);
	Sphere->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	Sphere->SetSimulatePhysics(false); // �� ���S��~�I

	// �gCLEAR�h�e�L�X�g��\��
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());

		// �V���v����HUD�\��
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("CLEAR!"));
	}
}



void AEggPlayer::ControlBall(const FInputActionValue& Value)
{
	// input��Value��Vector2D�ɕϊ��ł���
	FVector2D V = Value.Get<FVector2D>();

	// Vector���v�Z����
	FVector ForceVector = FVector(V.Y, V.X, 0.0f) * Speed;

	// Sphere�ɗ͂�������
	Sphere->AddForce(ForceVector, TEXT("NONE"), true);
}


void AEggPlayer::Look(const FInputActionValue& Value)
{
	// input��Value��Vector2D�ɕϊ��ł���
	const FVector2D V = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(V.X);
		AddControllerPitchInput(-V.Y);

		// Pawn�������Ă���Control�̊p�x���擾����
		FRotator ControlRotate = GetControlRotation();

		// controller��Pitch�̊p�x�𐧌�����
		//double LimitPitchAngle = FMath::ClampAngle(ControlRotate.Pitch, -40.0f, -10.0f);

		// PlayerController�̊p�x��ݒ肷��
		//UGameplayStatics::GetPlayerController(this, 0)->SetControlRotation(FRotator(LimitPitchAngle, ControlRotate.Yaw, 0.0f));
	}
}

void AEggPlayer::Jump(const FInputActionValue& Value)
{
	// input��Value��bool�ɕϊ��ł���
	if (const bool V = Value.Get<bool>() && CanJump)
	{
		Sphere->AddImpulse(FVector(0.0f, 0.0f, JumpImpulse), TEXT("None"), true);
		CanJump = false;
	}
}


