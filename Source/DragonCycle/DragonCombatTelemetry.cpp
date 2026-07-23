#include "DragonCombatTelemetry.h"
#include "GameplayTagsManager.h"

namespace DragonTelemetry
{
    EDragonLocomotionState LocomotionFromStateTags(const FGameplayTagContainer& StateTags)
    {
        // Request the ini-declared SPEC 13.3 tags rather than redeclaring them.
        // ErrorIfNotFound is false: a missing tag here means the config drifted
        // from the spec, which the tag manager already warns about on load; we
        // simply fall through to Grounded rather than crash the instrument.
        static const FGameplayTag GrappledTag =
            FGameplayTag::RequestGameplayTag(FName(TEXT("State.Grappled")), /*ErrorIfNotFound=*/false);
        static const FGameplayTag AirborneTag =
            FGameplayTag::RequestGameplayTag(FName(TEXT("State.Airborne")), /*ErrorIfNotFound=*/false);

        // Grappled dominates airborne: a fighter locked in a grapple is neither
        // freely grounded nor freely flying.
        if (GrappledTag.IsValid() && StateTags.HasTag(GrappledTag))
        {
            return EDragonLocomotionState::Grappled;
        }
        if (AirborneTag.IsValid() && StateTags.HasTag(AirborneTag))
        {
            return EDragonLocomotionState::Airborne;
        }
        return EDragonLocomotionState::Grounded;
    }

    const TCHAR* WinConditionToString(EDragonWinCondition Condition)
    {
        switch (Condition)
        {
        case EDragonWinCondition::WingDisable:       return TEXT("wing_disable");
        case EDragonWinCondition::StaminaAttrition:  return TEXT("stamina_attrition");
        case EDragonWinCondition::LethalDamage:      return TEXT("lethal_damage");
        case EDragonWinCondition::TerrainKill:       return TEXT("terrain_kill");
        case EDragonWinCondition::OpponentDisengage: return TEXT("opponent_disengage");
        default:                                     return TEXT("invalid");
        }
    }

    bool IsValidWinCondition(uint8 RawValue)
    {
        // The five documented conditions are exactly the enumerators, so a value
        // is valid iff it is within the enum range.
        return RawValue <= static_cast<uint8>(EDragonWinCondition::OpponentDisengage);
    }
}
