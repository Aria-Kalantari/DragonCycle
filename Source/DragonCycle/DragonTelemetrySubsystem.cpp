#include "DragonTelemetrySubsystem.h"
#include "Engine/World.h"
#include "Misc/Paths.h"

double UDragonTelemetrySubsystem::NowSeconds() const
{
    if (const UWorld* World = GetWorld())
    {
        return World->GetTimeSeconds();
    }
    return 0.0;
}

void UDragonTelemetrySubsystem::Deinitialize()
{
    if (Writer.IsValid())
    {
        Writer->Close();
        Writer.Reset();
    }
    Super::Deinitialize();
}

bool UDragonTelemetrySubsystem::OpenLog(const FString& AbsolutePath)
{
    if (!Writer.IsValid())
    {
        Writer = MakeUnique<FDragonRoundSummaryWriter>();
    }

    FString Path = AbsolutePath;
    if (Path.IsEmpty())
    {
        Path = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Telemetry"), TEXT("RoundSummaries.jsonl"));
    }
    return Writer->Open(Path, /*bAppend=*/true);
}

void UDragonTelemetrySubsystem::CloseLog()
{
    if (Writer.IsValid())
    {
        Writer->Close();
    }
}

void UDragonTelemetrySubsystem::BeginRound(FName EncounterId, int32 RoundId, FName ArenaId,
                                           const FDragonFighterProfile& FighterA, const FDragonFighterProfile& FighterB)
{
    ActiveRound.BeginRound(EncounterId, RoundId, ArenaId, FighterA, FighterB, NowSeconds());
    bRoundActive = true;
}

void UDragonTelemetrySubsystem::EndRound(FName Winner, EDragonWinCondition WinCondition)
{
    if (!ensureMsgf(bRoundActive, TEXT("EndRound called with no active round.")))
    {
        return;
    }

    ActiveRound.Finalize(Winner, WinCondition, NowSeconds());
    bRoundActive = false;

    if (Writer.IsValid() && Writer->IsOpen())
    {
        Writer->Append(ActiveRound);
        Writer->Flush();
    }
}

void UDragonTelemetrySubsystem::RecordCombatEvent(const FDragonCombatEvent& Event)
{
    // Broadcast first so no consumer can observe a round summary that already
    // reflects an event they have not seen.
    OnCombatEvent.Broadcast(Event);

    if (bRoundActive)
    {
        ActiveRound.IngestEvent(Event);
    }
}

void UDragonTelemetrySubsystem::SampleOccupancy(FName Fighter, EDragonLocomotionState Locomotion, FIntPoint Cell,
                                                float DeltaSeconds, bool bInContact)
{
    if (bRoundActive)
    {
        ActiveRound.SampleOccupancy(Fighter, Locomotion, Cell, DeltaSeconds, bInContact);
    }
}

void UDragonTelemetrySubsystem::RecordStaminaExhaustion(FName Fighter)
{
    if (bRoundActive) { ActiveRound.RecordStaminaExhaustion(Fighter); }
}

void UDragonTelemetrySubsystem::RecordDisengage(FName Fighter)
{
    if (bRoundActive) { ActiveRound.RecordDisengage(Fighter); }
}

void UDragonTelemetrySubsystem::RecordReengage(FName Fighter)
{
    if (bRoundActive) { ActiveRound.RecordReengage(Fighter); }
}

void UDragonTelemetrySubsystem::RecordDiveAttempt(FName Fighter)
{
    if (bRoundActive) { ActiveRound.RecordDiveAttempt(Fighter); }
}

void UDragonTelemetrySubsystem::RecordDiveConversion(FName Fighter)
{
    if (bRoundActive) { ActiveRound.RecordDiveConversion(Fighter); }
}

void UDragonTelemetrySubsystem::RecordGrappleAttempt(FName Fighter, EDragonGrappleOutcome Outcome)
{
    if (bRoundActive) { ActiveRound.RecordGrappleAttempt(Fighter, Outcome); }
}

void UDragonTelemetrySubsystem::RecordAssessment(const FDragonAssessmentTracePoint& Point)
{
    if (bRoundActive) { ActiveRound.RecordAssessment(Point); }
}
