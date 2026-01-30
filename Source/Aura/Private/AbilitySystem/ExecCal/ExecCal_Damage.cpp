#include "AbilitySystem/ExecCal/ExecCal_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Aura/Aura.h"
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
	const FAuraGameplayTags AuraTags = FAuraGameplayTags::Get();

	TagToCaptureDef.Add(AuraTags.Attributes_Secondary_Armor, DamageStatics().ArmorDef);
	TagToCaptureDef.Add(AuraTags.Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagToCaptureDef.Add(AuraTags.Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagToCaptureDef.Add(AuraTags.Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagToCaptureDef.Add(AuraTags.Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);
	TagToCaptureDef.Add(AuraTags.Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagToCaptureDef.Add(AuraTags.Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagToCaptureDef.Add(AuraTags.Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagToCaptureDef.Add(AuraTags.Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagToCaptureDef.Add(AuraTags.Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);
	const UAbilitySystemComponent* SourceAsc = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetAsc = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceAsc ? SourceAsc->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetAsc ? TargetAsc->GetAvatarActor() : nullptr;

	int32 SourceLevel = 1;
	int32 TargetLevel = 1;
	//ICombatInterface* SourceCombatInterface = nullptr;
	ICombatInterface* TargetCombatInterface = nullptr;

	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourceLevel = ICombatInterface::Execute_GetCharacterLevel(SourceAvatar);
		//SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	}

	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetLevel = ICombatInterface::Execute_GetCharacterLevel(TargetAvatar);
		TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(Spec.GetContext().Get());
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	//处理Debuff相关

	for (const auto& Pair : AuraTags.DamageTypeToDebuff)
	{
		//检测对应伤害类型的伤害是否被设置
		if (Spec.GetSetByCallerMagnitude(Pair.Key, false, -1.f) == -1.f)continue;
		const float DebuffChance = Spec.GetSetByCallerMagnitude(AuraTags.Debuff_Chance, false, -1);
		float Resistance;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TagToCaptureDef[AuraTags.DamageTypeToResistance[Pair.Key]], EvaluateParams,
		                                                           Resistance);
		Resistance = FMath::Max(Resistance, 0.f);
		//是否成功施加Debuff
		if (FMath::RandRange(0.f, 100.f) <= DebuffChance * (100.f - Resistance) / 100.f)
		{
			EffectContext->SetDebuffAppliedSuccessfully(true);
			EffectContext->SetDamageType(Pair.Key);
			EffectContext->SetDebuffDamage(Spec.GetSetByCallerMagnitude(AuraTags.Debuff_Damage, false, -1));
			EffectContext->SetDebuffDuration(Spec.GetSetByCallerMagnitude(AuraTags.Debuff_Duration, false, -1));
			EffectContext->SetDebuffFrequency(Spec.GetSetByCallerMagnitude(AuraTags.Debuff_Frequency, false, -1));
		}
	}
	//处理直接伤害相关
	float Damage = 0;
	for (const auto& Pair : Spec.SetByCallerTagMagnitudes)
	{
		if (Pair.Key.MatchesTag(AuraTags.Damage))
		{
			const FGameplayEffectAttributeCaptureDefinition& ResistanceDef = TagToCaptureDef[AuraTags.DamageTypeToResistance[Pair.Key]];
			float Resistance;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResistanceDef, EvaluateParams, Resistance);
			Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
			Damage += Pair.Value * (100.f - Resistance) / 100.f;
		}
	}
	if (EffectContext->IsRadialDamage() && TargetCombatInterface)
	{
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(SourceAvatar);
		Damage = UAuraAbilitySystemLibrary::ApplyRadialDamageWithFalloff(
			TargetAvatar,
			Damage,
			1.f,
			EffectContext->GetRadialDamageOrigin(),
			EffectContext->GetRadialDamageInnerRadius(),
			EffectContext->GetRadialDamageOuterRadius(),
			1.f,
			SourceAvatar,
			nullptr,
			ECC_DamageTrace
		);
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

	EffectContext->SetIsBlockedHit(bBlocked);
	EffectContext->SetIsCriticalHit(bCriticalHit);


	const FGameplayModifierEvaluatedData ModifierData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(ModifierData);
}
