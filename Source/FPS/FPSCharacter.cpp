#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Math/RotationMatrix.h"
#include "Components/ArrowComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

#include "FPS/ActorComponent/InventoryComponent.h"
#include "FPS/ActorComponent/FootIKComponent.h"
#include "FPS/ActorComponent/CombatComponent.h"
#include "FPS/RenderTargetCharacter.h"
#include "FPS/HUD/FPSHUD.h"
#include "FPS/Item/Item.h"

AFPSCharacter::AFPSCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	TurnRateGamepad = 50.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxAcceleration = 2048.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 
	CameraBoom->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom);
	ThirdPersonCamera->SetActive(true);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), FName("headpoint"));
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetActive(true);

	WeaponDropPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponEquippedPoint"));
	WeaponDropPoint->SetupAttachment(GetMesh());

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	FootIKComponent = CreateDefaultSubobject<UFootIKComponent>(TEXT("FootIKComponent"));

	RenderTargetCharacter = CreateDefaultSubobject<ARenderTargetCharacter>(TEXT("RenderTargetCharacter"));

	WeaponArray.Init(nullptr, 2);
	WeaponSokcetNameArray.Emplace(FName("FirstWeaponPoint"));
	WeaponSokcetNameArray.Emplace(FName("SecondWeaponPoint"));
}
void AFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("ToggleInventory", IE_Pressed, this, &AFPSCharacter::ToggleInventory);
	PlayerInputComponent->BindAction("ToggleSpeed", IE_Pressed, this, &AFPSCharacter::ToggleSpeed);
	PlayerInputComponent->BindAction("ToggleSpeed", IE_Released, this, &AFPSCharacter::ToggleSpeed);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AFPSCharacter::GetItem);
	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &AFPSCharacter::DropWeapon);
	PlayerInputComponent->BindAction("FirstWeapon", IE_Pressed, this, &AFPSCharacter::ChangeToFirstWeapon);
	PlayerInputComponent->BindAction("SecondWeapon", IE_Pressed, this, &AFPSCharacter::ChangeToSecondWeapon);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AFPSCharacter::Aim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AFPSCharacter::Aim);
	PlayerInputComponent->BindAction("ChangeCamera", IE_Released, this, &AFPSCharacter::ChangeCamera);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AFPSCharacter::ShootPressed);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AFPSCharacter::ShootReleased);
	PlayerInputComponent->BindAction("Reload", IE_Released, this, &AFPSCharacter::Reload);


	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AFPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AFPSCharacter::LookUpAtRate);
}
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (RenderTargetCharacterClass)
	{
		RenderTargetCharacter = GetWorld()->SpawnActor<ARenderTargetCharacter>(RenderTargetCharacterClass, GetActorLocation(), FRotator(0.f, 0.f, 0.f));
		if (RenderTargetCharacter)
		{
			RenderTargetCharacter->SetOwner(this);
			RenderTargetCharacter->Init(this);
		}
	}

	FPSController = Cast<APlayerController>(GetController());
	if (FPSController)
	{
		FPSHUD = Cast<AFPSHUD>(FPSController->GetHUD());
	}
}
void AFPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (InventoryComponent)
	{
		InventoryComponent->Init(this);
	}
	if (CombatComponent)
	{
		CombatComponent->Init(this);
	}
	if (FootIKComponent)
	{
		FootIKComponent->Init(this);
	}
}
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pitch = GetControlRotation().Pitch;

	if (bEquipped && CurrentEquippedWeaponIndex != -1 && WeaponArray[CurrentEquippedWeaponIndex])
	{
		LeftHandTransform = WeaponArray[CurrentEquippedWeaponIndex]->GetItemMesh()->GetSocketTransform(FName("LeftHand"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;

		GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
	}
}


void AFPSCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}
void AFPSCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}
void AFPSCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}
void AFPSCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}



void AFPSCharacter::ToggleInventory()
{
	if (InventoryComponent == nullptr || CombatComponent == nullptr || CombatComponent->GetCurrentCombatState() != ECombatState::ECS_Idle) return;

	bInventoryOpen = InventoryComponent->SetActiveInventory();
}


void AFPSCharacter::ToggleSpeed()
{
	bJog = !bJog;
	if (bJog)
	{
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}


// 인벤토리에 아이템 넣기
void AFPSCharacter::GetItem()
{
	if (CombatComponent)
	{
		CombatComponent->GetItem();
	}
}
// 무기 장착
void AFPSCharacter::EquipWeaponFromSlot(UItemObject* ItemObject, int32 Index)
{
	if (CombatComponent)
	{
		CombatComponent->EquipWeaponFromSlot(ItemObject, Index);
	}
}
// 무기 버리기
void AFPSCharacter::DropWeapon()
{
	if (CombatComponent)
	{
		CombatComponent->DropWeapon();
	}
}



void AFPSCharacter::ChangeToFirstWeapon()
{
	if (CombatComponent)
	{
		CombatComponent->ChangeToFirstWeapon();
	}
}
void AFPSCharacter::ChangeToSecondWeapon()
{
	if (CombatComponent)
	{
		CombatComponent->ChangeToSecondWeapon();
	}
}


void AFPSCharacter::Aim()
{
	if (CombatComponent)
	{
		CombatComponent->Aim();
	}
}


void AFPSCharacter::ChangeCamera()
{
	if (FirstPersonCamera == nullptr || ThirdPersonCamera == nullptr) return;

	bFirstPersonCamera = !bFirstPersonCamera;
	if (bFirstPersonCamera)
	{
		FirstPersonCamera->SetActive(true);
		ThirdPersonCamera->SetActive(false);
	}
	else
	{
		FirstPersonCamera->SetActive(false);
		ThirdPersonCamera->SetActive(true);
	}
}


UCameraComponent* AFPSCharacter::GetCurrentCamera()
{
	if (bFirstPersonCamera && FirstPersonCamera)
	{
		return FirstPersonCamera;
	}
	else if(ThirdPersonCamera)
	{
		return ThirdPersonCamera;
	}

	return nullptr;
}


void AFPSCharacter::ShootPressed()
{
	if (CombatComponent && !bInventoryOpen)
	{
		CombatComponent->ShootPressed();
	}
}
void AFPSCharacter::ShootReleased()
{
	if (CombatComponent && !bInventoryOpen)
	{
		CombatComponent->ShootReleased();
	}
}


void AFPSCharacter::Reload()
{
	if (CombatComponent && !bInventoryOpen)
	{
		CombatComponent->Reload();
	}
}


void AFPSCharacter::SetEquippedWeapon(AItem* Weapon)
{
	if (CombatComponent)
	{
		CombatComponent->SetEquippedWeapon(Weapon);
	}
}


void AFPSCharacter::AddRotationYaw(float Yaw)
{
	if (RenderTargetCharacter)
	{
		RenderTargetCharacter->AddRotationYaw(Yaw);
	}
}


void AFPSCharacter::FootStepNotify(const FName& FootName)
{
	FTransform FootTransform = GetMesh()->GetSocketTransform(FootName);

	FHitResult HitResult;
	FVector Start = FootTransform.GetLocation();
	FVector End = Start - FVector(0.f, 0.f, FootStepLineTraceDistance);
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(this);
	FQP.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic, FQP);
	if (HitResult.bBlockingHit)
	{
		UPhysicalMaterial* PhysicalMaterial = HitResult.PhysMaterial.Get();
		if (PhysicalMaterial == nullptr) return;

		switch (UPhysicalMaterial::DetermineSurfaceType(PhysicalMaterial))
		{
		case EPhysicalSurface::SurfaceType1 : // Wood
			if(WoodParticle)
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, WoodParticle, HitResult.ImpactPoint);
			if (WoodSound)
				UGameplayStatics::SpawnSoundAtLocation(this, WoodSound, HitResult.ImpactPoint);
			break;

		case EPhysicalSurface::SurfaceType2: // Stone
			if (StoneParticle)
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, StoneParticle, HitResult.ImpactPoint);
			if (StoneSound)
				UGameplayStatics::SpawnSoundAtLocation(this, StoneSound, HitResult.ImpactPoint);
			break;

		case EPhysicalSurface::SurfaceType3: // Sand
			if (SandParticle)
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SandParticle, HitResult.ImpactPoint);
			if (SandSound)
				UGameplayStatics::SpawnSoundAtLocation(this, SandSound, HitResult.ImpactPoint);
			break;
		}
	}
}