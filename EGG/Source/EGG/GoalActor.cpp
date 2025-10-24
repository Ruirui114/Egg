#include "GoalActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EggPlayer.h"

AGoalActor::AGoalActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetBoxExtent(FVector(100, 100, 100));
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AGoalActor::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGoalActor::OnOverlapBegin);
}

void AGoalActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// AEggPlayer�ƐڐG������
	if (AEggPlayer* Player = Cast<AEggPlayer>(OtherActor))
	{
		Player->OnGoalReached(); // �� �v���C���[�ɒm�点��
	}
}
