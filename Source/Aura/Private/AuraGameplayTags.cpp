#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	//获取全局唯一的标签管理器，添加原生标签
	//Primary Attributes
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Primary.Strength")), FString(TEXT("力量")));
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Primary.Intelligence")), FString(TEXT("智力")));
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Primary.Resilience")), FString(TEXT("韧性")));
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Primary.Vigor")), FString(TEXT("活力")));

	//Secondary Attributes
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.Armor")), FString(TEXT("减少收到的伤害，提升格挡几率")));
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.ArmorPenetration")), FString(TEXT("护甲穿透")));
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.BlockChance")), FString(TEXT("格挡几率")));
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.CriticalHitChance")), FString(TEXT("暴击几率")));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.CriticalHitDamage")), FString(TEXT("暴击伤害")));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.CriticalHitResistance")), FString(TEXT("暴击抗性")));
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.HealthRegeneration")), FString(TEXT("每秒恢复生命值")));
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.ManaRegeneration")), FString(TEXT("每秒恢复法力值")));
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.MaxHealth")), FString(TEXT("最大生命值")));
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.MaxMana")), FString(TEXT("最大法力值")));
	//Input Tag
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.LMB")), FString(TEXT("鼠标左键")));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.RMB")), FString(TEXT("鼠标右键")));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.1")), FString(TEXT("数字键1")));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.2")), FString(TEXT("数字键2")));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.3")), FString(TEXT("数字键3")));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.4")), FString(TEXT("数字键4")));
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage")), FString(TEXT("伤害")));
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Effects.HitReact")), FString(TEXT("受击反应")));
}
