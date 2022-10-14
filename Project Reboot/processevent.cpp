#include "processevent.h"
#include "helper.h"
#include "inventory.h"

bool ServerAcknowledgePossession(UObject* Object, UFunction* Function, void* Parameters)
{
	struct SAP_Params
	{
		UObject* P;
	};

	auto Params = (SAP_Params*)Parameters;
	if (Params)
	{
		auto Pawn = Params->P; // (UObject*)Parameters;

		static auto AcknowledgedPawnOffset = Object->GetOffset("AcknowledgedPawn");
		std::cout << "AcknowledgedPawnOffset: " << AcknowledgedPawnOffset << '\n';
		*(UObject**)(__int64(Object) + AcknowledgedPawnOffset) = Pawn;
		std::cout << "Set Pawn!\n";
	}

	return false;
}

bool HandleStartingNewPlayer(UObject* Object, UFunction* Function, void* Parameters)
{
	UObject* PlayerController = *(UObject**)Parameters;

	if (PlayerController)
	{
		if (Fortnite_Version < 7.4)
		{
			static const auto QuickBarsClass = FindObject("Class /Script/FortniteGame.FortQuickBars");
			static auto QuickBarsOffset = PlayerController->GetOffset("QuickBars");

			*(UObject**)(__int64(PlayerController) + QuickBarsOffset) = Helper::Easy::SpawnActor(QuickBarsClass, FVector(), FRotator(), PlayerController);
		}

		auto PlayerState = Helper::GetPlayerStateFromController(PlayerController);

		static auto bHasServerFinishedLoadingOffset = PlayerController->GetOffset("bHasServerFinishedLoading");
		*Get<bool>(PlayerController, bHasServerFinishedLoadingOffset) = true;

		static auto bHasStartedPlayingOffset = PlayerState->GetOffset("bHasStartedPlaying");
		*Get<bool>(PlayerState, bHasStartedPlayingOffset) = true;

		auto NewPawn = Helper::SpawnPawn(PlayerController, FVector{ 1250, 1818, 3284 }, true);

		static auto PickaxeDef = FindObject("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
		auto PickaxeInstance = Inventory::GiveItem(PlayerController, PickaxeDef, EFortQuickBars::Primary, 0);
	}

	return false;
}

bool ReadyToStartMatch(UObject* GameMode, UFunction* Function, void* Parameters)
{
	if (GameMode)
	{
		static auto GameStateOffset = GameMode->GetOffset("GameState");
		auto GameState = *Get<UObject*>(GameMode, GameStateOffset);

		static auto GamePhaseOffset = GameState->GetOffset("GamePhase");
		*Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::None;
	}

	return false;
}








void AddHook(const std::string& str, std::function<bool(UObject*, UFunction*, void*)> func)
{
	auto funcObject = FindObject<UFunction>(str);

	if (!funcObject)
		std::cout << "Unable to find Function: " << str << '\n';
	else
		FunctionsToHook.insert({ funcObject, func });
}

void ProcessEventDetour(UObject* Object, UFunction* Function, void* Parameters)
{
	if (!Object || !Function)
		return;

	auto FunctionName = Function->GetFullName();

	if (!strstr(FunctionName.c_str(), ("EvaluateGraphExposedInputs")) &&
		!strstr(FunctionName.c_str(), ("Tick")) &&
		!strstr(FunctionName.c_str(), ("OnSubmixEnvelope")) &&
		!strstr(FunctionName.c_str(), ("OnSubmixSpectralAnalysis")) &&
		!strstr(FunctionName.c_str(), ("OnMouse")) &&
		!strstr(FunctionName.c_str(), ("Pulse")) &&
		!strstr(FunctionName.c_str(), ("BlueprintUpdateAnimation")) &&
		!strstr(FunctionName.c_str(), ("BlueprintPostEvaluateAnimation")) &&
		!strstr(FunctionName.c_str(), ("BlueprintModifyCamera")) &&
		!strstr(FunctionName.c_str(), ("BlueprintModifyPostProcess")) &&
		!strstr(FunctionName.c_str(), ("Loop Animation Curve")) &&
		!strstr(FunctionName.c_str(), ("UpdateTime")) &&
		!strstr(FunctionName.c_str(), ("GetMutatorByClass")) &&
		!strstr(FunctionName.c_str(), ("UpdatePreviousPositionAndVelocity")) &&
		!strstr(FunctionName.c_str(), ("IsCachedIsProjectileWeapon")) &&
		!strstr(FunctionName.c_str(), ("LockOn")) &&
		!strstr(FunctionName.c_str(), ("GetAbilityTargetingLevel")) &&
		!strstr(FunctionName.c_str(), ("ReadyToEndMatch")) &&
		!strstr(FunctionName.c_str(), ("ReceiveDrawHUD")) &&
		!strstr(FunctionName.c_str(), ("OnUpdateDirectionalLightForTimeOfDay")) &&
		!strstr(FunctionName.c_str(), ("GetSubtitleVisibility")) &&
		!strstr(FunctionName.c_str(), ("GetValue")) &&
		!strstr(FunctionName.c_str(), ("InputAxisKeyEvent")) &&
		!strstr(FunctionName.c_str(), ("ServerTouchActiveTime")) &&
		!strstr(FunctionName.c_str(), ("SM_IceCube_Blueprint_C")) &&
		!strstr(FunctionName.c_str(), ("OnHovered")) &&
		!strstr(FunctionName.c_str(), ("OnCurrentTextStyleChanged")) &&
		!strstr(FunctionName.c_str(), ("OnButtonHovered")) &&
		!strstr(FunctionName.c_str(), ("ExecuteUbergraph_ThreatPostProcessManagerAndParticleBlueprint")) &&
		!strstr(FunctionName.c_str(), ("UpdateCamera")) &&
		!strstr(FunctionName.c_str(), ("GetMutatorContext")) &&
		!strstr(FunctionName.c_str(), ("CanJumpInternal")) &&
		!strstr(FunctionName.c_str(), ("OnDayPhaseChanged")) &&
		!strstr(FunctionName.c_str(), ("Chime")) &&
		!strstr(FunctionName.c_str(), ("ServerMove")) &&
		!strstr(FunctionName.c_str(), ("OnVisibilitySetEvent")) &&
		!strstr(FunctionName.c_str(), "ReceiveHit") &&
		!strstr(FunctionName.c_str(), "ReadyToStartMatch") &&
		!strstr(FunctionName.c_str(), "ClientAckGoodMove") &&
		!strstr(FunctionName.c_str(), "Prop_WildWest_WoodenWindmill_01") &&
		!strstr(FunctionName.c_str(), "ContrailCheck") &&
		!strstr(FunctionName.c_str(), "B_StockBattleBus_C") &&
		!strstr(FunctionName.c_str(), "Subtitles.Subtitles_C.") &&
		!strstr(FunctionName.c_str(), "/PinkOatmeal/PinkOatmeal_") &&
		!strstr(FunctionName.c_str(), "BP_SpectatorPawn_C") &&
		!strstr(FunctionName.c_str(), "FastSharedReplication") &&
		!strstr(FunctionName.c_str(), "OnCollisionHitEffects") &&
		!strstr(FunctionName.c_str(), "BndEvt__SkeletalMesh") &&
		!strstr(FunctionName.c_str(), ".FortAnimInstance.AnimNotify_") &&
		!strstr(FunctionName.c_str(), "OnBounceAnimationUpdate") &&
		!strstr(FunctionName.c_str(), "ShouldShowSoundIndicator") &&
		!strstr(FunctionName.c_str(), "Primitive_Structure_AmbAudioComponent_C") &&
		!strstr(FunctionName.c_str(), "PlayStoppedIdleRotationAudio") &&
		!strstr(FunctionName.c_str(), "UpdateOverheatCosmetics") &&
		!strstr(FunctionName.c_str(), "StormFadeTimeline__UpdateFunc") &&
		!strstr(FunctionName.c_str(), "BindVolumeEvents") &&
		!strstr(FunctionName.c_str(), "UpdateStateEvent") &&
		!strstr(FunctionName.c_str(), "VISUALS__UpdateFunc") &&
		!strstr(FunctionName.c_str(), "Flash__UpdateFunc") &&
		!strstr(FunctionName.c_str(), "SetCollisionEnabled") &&
		!strstr(FunctionName.c_str(), "SetIntensity") &&
		!strstr(FunctionName.c_str(), "Storm__UpdateFunc") &&
		!strstr(FunctionName.c_str(), "CloudsTimeline__UpdateFunc") &&
		!strstr(FunctionName.c_str(), "SetRenderCustomDepth") &&
		!strstr(FunctionName.c_str(), "K2_UpdateCustomMovement") &&
		!strstr(FunctionName.c_str(), "AthenaHitPointBar_C.Update") &&
		!strstr(FunctionName.c_str(), "ExecuteUbergraph_Farm_WeatherVane_01") &&
		!strstr(FunctionName.c_str(), "HandleOnHUDElementVisibilityChanged") &&
		!strstr(FunctionName.c_str(), "ExecuteUbergraph_Fog_Machine"))
	{
		std::cout << ("Function called: ") << FunctionName << '\n';
	}

	for (auto& Func : FunctionsToHook)
	{
		if (Function == Func.first)
		{
			if (Func.second(Object, Function, Parameters)) // If the function returned true, then cancel default execution.
			{
				return;
			}
		}
	}

	return ProcessEventO(Object, Function, Parameters);
}