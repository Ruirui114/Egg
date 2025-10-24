// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "EggPlayer.generated.h"


class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class EGG_API AEggPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEggPlayer();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Hit Event��Binding����֐�
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/** Ball��Control���� */
	void ControlBall(const FInputActionValue& Value);

	/** ���_�𑀍삷�� */
	void Look(const FInputActionValue& Value);

	// �W�����v����
	void Jump(const FInputActionValue& Value);
	
public:
	void OnGoalReached(); // �S�[�����ɌĂ΂��֐�

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Character�p��StaticMesh : Sphere */
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Sphere;

	/** Camera��z�u���邽�߂�SpringArm */
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	/** SpringArm�̐�[�ɔz�u����J���� */
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Control Input Action */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ControlAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	bool bIsGoalReached = false;

	// ���x
	float Speed = 200.0f;

	// �̗�
	float Health = 100.0f;

	// �W�����v�ɉ������
	float JumpImpulse = 2000.0f;

	// �W�����v���ł��邩���肷��t���O
	bool CanJump = false;
};
