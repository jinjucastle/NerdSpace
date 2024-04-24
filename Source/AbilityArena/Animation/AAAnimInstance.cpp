// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AAAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"

UAAAnimInstance::UAAAnimInstance()
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.0f;

}

void UAAAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UAAAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement) 
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);

		//ver 0.1.0 C
		FRotator ActorRotator = GetOwningActor()->GetActorRotation();
		Direction=CalculateDirection(Velocity, ActorRotator);
	}

}
