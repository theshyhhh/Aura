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
	//Meta Attributes
	GameplayTags.Attributes_Meta_InComingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Meta.InComingXP")), FString(TEXT("传入的经验值元数据")));
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
	GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.Passive.1")), FString(TEXT("被动技能1")));
	GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.Passive.2")), FString(TEXT("被动技能2")));
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
	//Debuff标签
	GameplayTags.Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Debuff.Burn")), FString(TEXT("燃烧状态")));
	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Debuff.Stun")), FString(TEXT("眩晕状态")));
	GameplayTags.Debuff_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Debuff.Arcane")), FString(TEXT("奥术状态")));
	GameplayTags.Debuff_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Debuff.Physical")), FString(TEXT("物理状态")));
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Debuff.Chance")), FString(TEXT("Debuff施加几率")));
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Debuff.Damage")), FString(TEXT("Debuff造成的伤害")));
	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Debuff.Duration")), FString(TEXT("Debuff持续时间")));
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Debuff.Frequency")), FString(TEXT("Debuff触发间隔")));
	GameplayTags.DamageTypeToDebuff.Add(GameplayTags.Damage_Fire, GameplayTags.Debuff_Burn);
	GameplayTags.DamageTypeToDebuff.Add(GameplayTags.Damage_Lightning, GameplayTags.Debuff_Stun);
	GameplayTags.DamageTypeToDebuff.Add(GameplayTags.Damage_Arcane, GameplayTags.Debuff_Arcane);
	GameplayTags.DamageTypeToDebuff.Add(GameplayTags.Damage_Physical, GameplayTags.Debuff_Physical);
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
	GameplayTags.Abilities_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.None")), FString(TEXT("无")));
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Attack")), FString(TEXT("攻击能力标签")));
	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Summon")), FString(TEXT("召唤能力标签")));
	GameplayTags.Abilities_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Fire.FireBolt")), FString(TEXT("火球术标签")));
	GameplayTags.Abilities_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Lightning.Electrocute")), FString(TEXT("闪电术标签")));
	GameplayTags.Abilities_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.HitReact")), FString(TEXT("受击反应标签")));
	GameplayTags.Abilities_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Status.Locked")), FString(TEXT("技能为锁定状态")));
	GameplayTags.Abilities_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Status.Eligible")), FString(TEXT("技能为可解锁状态")));
	GameplayTags.Abilities_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Status.Unlocked")), FString(TEXT("技能为已解锁状态")));
	GameplayTags.Abilities_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Status.Equipped")), FString(TEXT("技能为已装备状态")));
	GameplayTags.Abilities_Type_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Type.Offensive")), FString(TEXT("主动技能")));
	GameplayTags.Abilities_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Type.Passive")), FString(TEXT("被动技能")));
	GameplayTags.Abilities_Type_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Type.None")), FString(TEXT("无类型技能")));

	//冷却标签
	GameplayTags.Cooldown_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Cooldown.Fire.FireBolt")), FString(TEXT("火球术冷却标签")));

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

	//阻挡输入标签
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TEXT("Player.Block.InputPressed")));
	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TEXT("Player.Block.InputHeld")));
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TEXT("Player.Block.InputReleased")));
	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TEXT("Player.Block.CursorTrace")));
}
