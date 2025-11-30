#include "AbilitySystem/ExecCal/ExecCal_Damage.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

//仅用在C++中的原始结构体，不加F为了区分U结构体
struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance)

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCal_Damage::UExecCal_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCal_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagToCaptureDef;

	TagToCaptureDef.Add(FAuraGameplayTags::Get().Attributes_Secondary_Armor, DamageStatics().ArmorDef);
	TagToCaptureDef.Add(FAuraGameplayTags::Get().Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagToCaptureDef.Add(FAuraGameplayTags::Get().Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagToCaptureDef.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagToCaptureDef.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);
	TagToCaptureDef.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagToCaptureDef.Add(FAuraGameplayTags::Get().Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagToCaptureDef.Add(FAuraGameplayTags::Get().Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagToCaptureDef.Add(FAuraGameplayTags::Get().Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagToCaptureDef.Add(FAuraGameplayTags::Get().Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);
	const UAbilitySystemComponent* SourceAsc = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetAsc = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceAsc ? SourceAsc->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetAsc ? TargetAsc->GetAvatarActor() : nullptr;

	int32 SourceLevel = 1;
	int32 TargetLevel = 1;

	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourceLevel = ICombatInterface::Execute_GetCharacterLevel(SourceAvatar);
	}

	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetLevel = ICombatInterface::Execute_GetCharacterLevel(TargetAvatar);
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Damage = 0;
	for (const auto& Pair : Spec.SetByCallerTagMagnitudes)
	{
		if (Pair.Key.MatchesTag(FAuraGameplayTags::Get().Damage))
		{
			const FGameplayEffectAttributeCaptureDefinition& ResistanceDef = TagToCaptureDef[FAuraGameplayTags::Get()
				.DamageTypeToResistance[Pair.Key]];
			float Resistance;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResistanceDef, EvaluateParams, Resistance);
			Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
			Damage += Pair.Value * (100.f - Resistance) / 100.f;
		}
	}
	//捕获目标的格挡几率属性，判断是否成功格挡，如果成功，伤害减半
	float BlockChance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParams, BlockChance);
	BlockChance = FMath::Max(BlockChance, 0.f);
	const bool bBlocked = FMath::RandRange(1.f, 100.f) < BlockChance;
	Damage = bBlocked ? Damage / 2 : Damage;

	float TargetArmor;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParams, TargetArmor);
	TargetArmor = FMath::Max(TargetArmor, 0.f);

	float SourceArmorPenetration;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParams, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(SourceArmorPenetration, 0.f);

	float SourceCriticalHitChance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluateParams, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max(SourceCriticalHitChance, 0.f);

	float SourceCriticalHitDamage;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluateParams, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max(SourceCriticalHitDamage, 0.f);

	float TargetCriticalHitResistance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluateParams, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max(TargetCriticalHitResistance, 0.f);

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorCoefficientCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("Armor"), FString());
	const FRealCurve* ArmorPenetrationCoefficientCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(
		FName("ArmorPenetration"), FString());
	const FRealCurve* CriticalHitResistanceCoefficientCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(
		FName("CriticalHitResistance"), FString());


	const float ArmorPenetrationCoefficient = ArmorPenetrationCoefficientCurve->Eval(SourceLevel);
	const float ArmorCoefficient = ArmorCoefficientCurve->Eval(TargetLevel);
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCoefficientCurve->Eval(
		TargetLevel);

	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100;
	Damage *= (100 - EffectiveArmor * ArmorCoefficient) / 100;

	const float EffectiveCriticalChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(0.f, 100.f) < EffectiveCriticalChance;

	Damage = bCriticalHit ? Damage * (2 + SourceCriticalHitDamage / 100) : Damage;
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(Spec.GetContext().Get());
	EffectContext->SetIsBlockedHit(bBlocked);
	EffectContext->SetIsCriticalHit(bCriticalHit);


	const FGameplayModifierEvaluatedData ModifierData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(ModifierData);
}
