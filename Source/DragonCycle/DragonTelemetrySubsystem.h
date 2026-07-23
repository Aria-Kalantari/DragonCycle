#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DragonCombatTelemetry.h"
#include "DragonRoundSummary.h"
#include "DragonTelemetrySubsystem.generated.h"

/** The single combat-event broadcast. Animation, VFX, audio, UI, AI, and
 *  telemetry all subscribe here. There is no second event source (ADR-010). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDragonCombatEventSignature, const FDragonCombatEvent&, Event);

/**
 * The one origin for combat events (ADR-010, Docs/TELEMETRY.md). Every combat
 * event is routed through RecordCombatEvent, which broadcasts to all consumers
 * and folds the event into the active round summary. The round summary is
 * written to disk here as well, so this subsystem owns both the event schema and
 * the round-summary writer described in the telemetry instrumentation order.
 *
 * A WorldSubsystem so an AI-vs-AI batch run gets one instance per world and can
 * open a per-world log. It computes nothing about combat itself; it only records
 * and routes what the combat, movement, and AI systems report.
 */
UCLASS()
class DRAGONCYCLE_API UDragonTelemetrySubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    /** Subscribe to consume the single combat event (presentation, AI, tests). */
    UPROPERTY(BlueprintAssignable, Category="Dragon|Telemetry")
    FDragonCombatEventSignature OnCombatEvent;

    // --- Log lifecycle ----------------------------------------------------

    /** Open the round-summary log. Absolute path; defaults under Saved/Telemetry
     *  when Empty. Returns false if the writer could not open. */
    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    bool OpenLog(const FString& AbsolutePath);

    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void CloseLog();

    UFUNCTION(BlueprintPure, Category="Dragon|Telemetry")
    bool IsLogOpen() const { return Writer.IsValid() && Writer->IsOpen(); }

    // --- Round lifecycle --------------------------------------------------

    /** Start a round. Start time defaults to the world time. */
    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void BeginRound(FName EncounterId, int32 RoundId, FName ArenaId,
                    const FDragonFighterProfile& FighterA, const FDragonFighterProfile& FighterB);

    /** Finalize the active round, append it to the open log, and flush. */
    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void EndRound(FName Winner, EDragonWinCondition WinCondition);

    UFUNCTION(BlueprintPure, Category="Dragon|Telemetry")
    bool IsRoundActive() const { return bRoundActive; }

    // --- The single event path --------------------------------------------

    /** Record one combat event: broadcast it to every consumer, then fold it into
     *  the active round summary. This is the only way an event enters the game. */
    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void RecordCombatEvent(const FDragonCombatEvent& Event);

    // --- Round-summary passthroughs ---------------------------------------
    // Thin routes onto the active round so callers never touch a second summary.

    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void SampleOccupancy(FName Fighter, EDragonLocomotionState Locomotion, FIntPoint Cell,
                         float DeltaSeconds, bool bInContact);

    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void RecordStaminaExhaustion(FName Fighter);

    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void RecordDisengage(FName Fighter);

    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void RecordReengage(FName Fighter);

    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void RecordDiveAttempt(FName Fighter);

    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void RecordDiveConversion(FName Fighter);

    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void RecordGrappleAttempt(FName Fighter, EDragonGrappleOutcome Outcome);

    UFUNCTION(BlueprintCallable, Category="Dragon|Telemetry")
    void RecordAssessment(const FDragonAssessmentTracePoint& Point);

    /** Read-only view of the round currently accumulating. */
    const FDragonRoundSummary& GetActiveRound() const { return ActiveRound; }

    // --- UWorldSubsystem --------------------------------------------------

    virtual void Deinitialize() override;

private:
    /** Best-effort world time; 0 before a world exists (e.g. in a unit test). */
    double NowSeconds() const;

    UPROPERTY(Transient)
    FDragonRoundSummary ActiveRound;

    bool bRoundActive = false;

    TUniquePtr<FDragonRoundSummaryWriter> Writer;
};
