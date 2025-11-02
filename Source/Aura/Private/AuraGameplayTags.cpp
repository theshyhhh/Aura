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
	//伤害类型标签
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage")), FString(TEXT("伤害")));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage.Fire")), FString(TEXT("火焰类型伤害")));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage.Lightning")), FString(TEXT("闪电类型伤害")));
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage.Arcane")), FString(TEXT("奥术类型伤害")));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage.Physical")), FString(TEXT("物理类型伤害")));
	//伤害抗性标签
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Resistance.Fire")), FString(TEXT("火焰伤害抵抗")));
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Resistance.Lightning")), FString(TEXT("闪电伤害抵抗")));
	GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Resistance.Arcane")), FString(TEXT("奥术伤害抵抗")));
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Resistance.Physical")), FString(TEXT("物理伤害抵抗")));
	//添加伤害类型与抗性类型之间的映射
	GameplayTags.DamageTypeToResistance.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypeToResistance.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypeToResistance.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypeToResistance.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);

	//处于受击状态时添加的标签
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Effects.HitReact")), FString(TEXT("受击反应")));
	//能力标签
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Attack")), FString(TEXT("攻击能力标签")));
	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Summon")), FString(TEXT("召唤能力标签")));

	//攻击插槽的标签，如用武器攻击，左右手攻击，传输的Tag
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("CombatSocket.Weapon")), FString(TEXT("用武器攻击")));
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("CombatSocket.RightHand")), FString(TEXT("用右手攻击")));
	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("CombatSocket.LeftHand")), FString(TEXT("用左手攻击")));
	GameplayTags.CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("CombatSocket.Tail")), FString(TEXT("用尾巴攻击")));

	//攻击动画的Tag
	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Montage.Attack.1")));
	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Montage.Attack.2")));
	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Montage.Attack.3")));
	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Montage.Attack.4")));
}
