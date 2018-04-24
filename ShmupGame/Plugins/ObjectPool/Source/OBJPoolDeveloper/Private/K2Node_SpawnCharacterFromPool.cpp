/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "K2Node_SpawnCharacterFromPool.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_CallFunction.h"
#include "EditorCategoryUtils.h"
#include "K2Node_EnumLiteral.h"
#include "UObject/UnrealType.h"
#include "KismetCompilerMisc.h"
#include "Engine/EngineTypes.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraph.h"
#include "KismetCompiler.h"
#include "K2Node_Select.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FK2Node_SpawnCharacterFromPoolHelper {
	static FName PoolPinName;
	static FName OwnerPinName;
	static FName SuccessPinName;
	static FName CharacterPinName;
	static FName ReconstructPinName;
	static FName WorldContextPinName;
	static FName SpawnOptionsPinName;
	static FName SpawnTransformPinName;
	static FName NoCollisionFailPinName;
	static FName CollisionHandlingOverridePinName;
};

FName FK2Node_SpawnCharacterFromPoolHelper::OwnerPinName(TEXT("Owner"));
FName FK2Node_SpawnCharacterFromPoolHelper::PoolPinName(TEXT("ObjectPool"));
FName FK2Node_SpawnCharacterFromPoolHelper::CharacterPinName(TEXT("Character"));
FName FK2Node_SpawnCharacterFromPoolHelper::ReconstructPinName(TEXT("Reconstruct"));
FName FK2Node_SpawnCharacterFromPoolHelper::SuccessPinName(TEXT("SpawnSuccessful"));
FName FK2Node_SpawnCharacterFromPoolHelper::SpawnOptionsPinName(TEXT("SpawnOptions"));
FName FK2Node_SpawnCharacterFromPoolHelper::SpawnTransformPinName(TEXT("SpawnTransform"));
FName FK2Node_SpawnCharacterFromPoolHelper::WorldContextPinName(TEXT("WorldContextObject"));
FName FK2Node_SpawnCharacterFromPoolHelper::NoCollisionFailPinName(TEXT("SpawnEvenIfColliding"));	
FName FK2Node_SpawnCharacterFromPoolHelper::CollisionHandlingOverridePinName(TEXT("CollisionHandlingOverride"));

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UK2Node_SpawnCharacterFromPool::UK2Node_SpawnCharacterFromPool(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer) {
	NodeTooltip = LOCTEXT("NodeTooltip", "Attempts to Spawn from Pool an Inactive Character. Creates then Spawns a fully new Character if there's none available in the Pool and 'Instantiate On Demand' is checked on Project Settings.");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FText UK2Node_SpawnCharacterFromPool::GetNodeTitle(ENodeTitleType::Type TitleType) const {
	FText NodeTitle = NSLOCTEXT("K2Node","SpawnCharacterFromPool_BaseTitle","Spawn Character From Object-Pool ...");
	//
	if (TitleType != ENodeTitleType::MenuTitle) {
		UEdGraphPin* PoolPin = GetPoolPin();
		if (PoolPin && PoolPin->LinkedTo.Num()>0) {
			auto LinkedPin = PoolPin->LinkedTo[0];
			UClass* Class = (LinkedPin) ? Cast<UClass>(LinkedPin->PinType.PinSubCategoryObject.Get()) : nullptr;
			UCharacterPool* Pool = Cast<UCharacterPool>(Class->ClassDefaultObject);
			if (Pool && Pool->TemplateClass->IsValidLowLevelFast() && CachedNodeTitle.IsOutOfDate(this)) {
				FText Name = Pool->TemplateClass->GetDisplayNameText();
				FFormatNamedArguments Args; Args.Add(TEXT("Name"),Name);
				CachedNodeTitle.SetCachedText(FText::Format(NSLOCTEXT("K2Node","SpawnCharacterFromPool_Title","Spawn Character From Pool :: {Name}"),Args),this);
		} NodeTitle = CachedNodeTitle;}
	} return NodeTitle;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UK2Node_SpawnCharacterFromPool::AllocateDefaultPins() {
	UScriptStruct* TransformStruct = TBaseStructure<FTransform>::Get();
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	UEnum* const CHO_Enum = FindObjectChecked<UEnum>(ANY_PACKAGE,TEXT("ESpawnActorCollisionHandlingMethod"),true);
	//
	/// Execution Pins.
	FEdGraphPinType PINT_Exec(K2Schema->PC_Exec,TEXT(""),nullptr,EPinContainerType::None,false,FEdGraphTerminalType());
	FEdGraphPinType PINT_Bool(K2Schema->PC_Boolean,TEXT(""),nullptr,EPinContainerType::None,false,FEdGraphTerminalType());
	FEdGraphPinType PINT_CHO_Enum(K2Schema->PC_Byte,TEXT(""),CHO_Enum,EPinContainerType::None,false,FEdGraphTerminalType());
	FEdGraphPinType PINT_Transform(K2Schema->PC_Struct,TEXT(""),TransformStruct,EPinContainerType::None,false,FEdGraphTerminalType());
	FEdGraphPinType PINT_Owner(K2Schema->PC_Object,TEXT(""),ACharacter::StaticClass(),EPinContainerType::None,false,FEdGraphTerminalType());
	FEdGraphPinType PINT_Object(K2Schema->PC_Object,TEXT(""),UObject::StaticClass(),EPinContainerType::None,false,FEdGraphTerminalType());
	FEdGraphPinType PINT_Result(K2Schema->PC_Object,TEXT(""),APooledCharacter::StaticClass(),EPinContainerType::None,false,FEdGraphTerminalType());
	FEdGraphPinType PINT_SpawnOptions(K2Schema->PC_Struct,TEXT(""),FPoolSpawnOptions::StaticStruct(),EPinContainerType::None,false,FEdGraphTerminalType());
	//
	UEdGraphPin* PIN_Spawn = CreatePin(EGPD_Input,PINT_Exec,K2Schema->PN_Execute);
	UEdGraphPin* PIN_Finish = CreatePin(EGPD_Output,PINT_Exec,K2Schema->PN_Then);
	//
	PIN_Spawn->PinFriendlyName = LOCTEXT("Spawn","Spawn");
	PIN_Finish->PinFriendlyName = LOCTEXT("Finished","Finished Spawning");
	//
	//
	/// World Context Pin.
	if (GetBlueprint()->ParentClass->HasMetaDataHierarchical(FBlueprintMetadata::MD_ShowWorldContextPin)) {
		CreatePin(EGPD_Input,PINT_Object,FK2Node_SpawnCharacterFromPoolHelper::WorldContextPinName);
	}///
	//
	/// Object-Pool Pin.
	UEdGraphPin* PIN_Pool = CreatePin(EGPD_Input,PINT_Object,FK2Node_SpawnCharacterFromPoolHelper::PoolPinName);
	//
	/// Owner Pin.
	UEdGraphPin* PIN_Owner = CreatePin(EGPD_Input,PINT_Owner,FK2Node_SpawnCharacterFromPoolHelper::OwnerPinName);
	PIN_Owner->bAdvancedView = true;
	//
	/// Collision Handling Pin.
	UEdGraphPin* const PIN_CollisionHandlingOverride = CreatePin(EGPD_Input,PINT_CHO_Enum,FK2Node_SpawnCharacterFromPoolHelper::CollisionHandlingOverridePinName);
	PIN_CollisionHandlingOverride->DefaultValue = CHO_Enum->GetNameStringByIndex(static_cast<int>(ESpawnActorCollisionHandlingMethod::Undefined));
	//
	/// Reconstruct Pin.
	UEdGraphPin* PIN_Transform = CreatePin(EGPD_Input,PINT_Transform,FK2Node_SpawnCharacterFromPoolHelper::SpawnTransformPinName);
	UEdGraphPin* PIN_Options = CreatePin(EGPD_Input,PINT_SpawnOptions,FK2Node_SpawnCharacterFromPoolHelper::SpawnOptionsPinName);
	UEdGraphPin* PIN_Reconstruct = CreatePin(EGPD_Input,PINT_Bool,FK2Node_SpawnCharacterFromPoolHelper::ReconstructPinName);
	UEdGraphPin* PIN_Success = CreatePin(EGPD_Output,PINT_Bool,FK2Node_SpawnCharacterFromPoolHelper::SuccessPinName);
	UEdGraphPin* PIN_Result = CreatePin(EGPD_Output,PINT_Result,K2Schema->PN_ReturnValue);
	//
	if (ENodeAdvancedPins::NoPins == AdvancedPinDisplay) {
		AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	} Super::AllocateDefaultPins();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UClass* UK2Node_SpawnCharacterFromPool::GetClassToSpawn(const TArray<UEdGraphPin*>* InPinsToSearch) const {
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;
	//
	UEdGraphPin* PoolPin = GetPoolPin(PinsToSearch);
	UClass* SpawnClass = nullptr;
	//
	if (PoolPin && (PoolPin->LinkedTo.Num()>0)) {
		auto LinkedPin = PoolPin->LinkedTo[0];
		UClass* PoolClass = (LinkedPin) ? Cast<UClass>(LinkedPin->PinType.PinSubCategoryObject.Get()) : nullptr;
		UCharacterPool* PoolComponent = Cast<UCharacterPool>(PoolClass->ClassDefaultObject);
		if (PoolComponent) {
			SpawnClass = PoolComponent->TemplateClass;
	}} return SpawnClass;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UK2Node_SpawnCharacterFromPool::CreatePinsForClass(UClass* InClass, TArray<UEdGraphPin*>* OutClassPins) {
	check(InClass);
	//
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	const UObject* const ClassDefaultObject = InClass->GetDefaultObject(false);
	//
	for (TFieldIterator<UProperty> PIT(InClass, EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		UClass* PropertyClass = CastChecked<UClass>(Property->GetOuter());
		//
		const bool bIsDelegate = Property->IsA(UMulticastDelegateProperty::StaticClass());
		const bool bIsExposedToSpawn = UEdGraphSchema_K2::IsPropertyExposedOnSpawn(Property);
		const bool bIsSettableExternally = !Property->HasAnyPropertyFlags(CPF_DisableEditOnInstance);
		//
		if (bIsExposedToSpawn && !Property->HasAnyPropertyFlags(CPF_Parm) && bIsSettableExternally && Property->HasAllPropertyFlags(CPF_BlueprintVisible) &&
			!bIsDelegate && (nullptr==FindPin(Property->GetFName())) && FBlueprintEditorUtils::PropertyStillExists(Property)
		){
			if (UEdGraphPin* Pin = CreatePin(EGPD_Input,NAME_None,Property->GetFName())) {
				K2Schema->ConvertPropertyToPinType(Property,Pin->PinType);
				if (OutClassPins) {OutClassPins->Add(Pin);}
				//
				if (ClassDefaultObject && K2Schema->PinDefaultValueIsEditable(*Pin)) {
					FString DefaultValueAsString;
					const bool bDefaultValueSet = FBlueprintEditorUtils::PropertyValueToString(Property,reinterpret_cast<const uint8*>(ClassDefaultObject),DefaultValueAsString);
					check(bDefaultValueSet); K2Schema->SetPinAutogeneratedDefaultValue(Pin, DefaultValueAsString);
				} K2Schema->ConstructBasicPinTooltip(*Pin, Property->GetToolTipText(), Pin->PinToolTip);
		}}//////
	}///
	//
	UEdGraphPin* ResultPin = GetResultPin();
	ResultPin->PinType.PinSubCategoryObject = InClass->GetAuthoritativeClass();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UK2Node_SpawnCharacterFromPool::ExpandNode(class FKismetCompilerContext &CompilerContext, UEdGraph* SourceGraph) {
	Super::ExpandNode(CompilerContext,SourceGraph);
	//
	static FName BeginSpawningBlueprintFuncName = GET_FUNCTION_NAME_CHECKED(UCharacterPool,BeginDeferredSpawnFromPool);
	static FName FinishSpawningFuncName = GET_FUNCTION_NAME_CHECKED(UCharacterPool,FinishDeferredSpawnFromPool);
	//
	UK2Node_SpawnCharacterFromPool* SpawnNode = this;
	UEdGraphPin* SpawnPoolPin = SpawnNode->GetPoolPin();
	UEdGraphPin* SpawnNodeExec = SpawnNode->GetExecPin();
	UEdGraphPin* SpawnNodeThen = SpawnNode->GetThenPin();
	UEdGraphPin* SpawnNodeResult = SpawnNode->GetResultPin();
	UEdGraphPin* SpawnSuccessPin = SpawnNode->GetSuccessPin();
	UEdGraphPin* SpawnNodeOwnerPin = SpawnNode->GetOwnerPin();
	UEdGraphPin* SpawnNodeOptions = SpawnNode->GetSpawnOptionsPin();
	UEdGraphPin* SpawnWorldContextPin = SpawnNode->GetWorldContextPin();
	UEdGraphPin* SpawnNodeTransform = SpawnNode->GetSpawnTransformPin();
	UEdGraphPin* SpawnNodeReconstructPin = SpawnNode->GetReconstructPin();
	UEdGraphPin* SpawnNodeCollisionHandlingOverride = GetCollisionHandlingOverridePin();
	//
	//
	UClass* ClassToSpawn = GetClassToSpawn();
	//
	if (!SpawnPoolPin||(SpawnPoolPin->LinkedTo.Num()==0)) {
		CompilerContext.MessageLog.Error(*LOCTEXT("SpawnCharacterFromPool_Pool_Error","Spawn Node: @@ must have an @@ specified!").ToString(),SpawnNode,SpawnPoolPin);
		SpawnNode->BreakAllNodeLinks();
	return;}
	//
	//
	UK2Node_CallFunction* CallBeginSpawnNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnNode,SourceGraph);
	CallBeginSpawnNode->FunctionReference.SetExternalMember(BeginSpawningBlueprintFuncName,UCharacterPool::StaticClass());
	CallBeginSpawnNode->AllocateDefaultPins();
	//
	UEdGraphPin* CallBeginExec = CallBeginSpawnNode->GetExecPin();
	UEdGraphPin* CallBeginResult = CallBeginSpawnNode->GetReturnValuePin();
	UEdGraphPin* CallBeginOwnerPin = CallBeginSpawnNode->FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::OwnerPinName);
	UEdGraphPin* CallBeginCharacterPoolPin = CallBeginSpawnNode->FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::PoolPinName);
	UEdGraphPin* CallBeginSuccessPin = CallBeginSpawnNode->FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::SuccessPinName);
	UEdGraphPin* CallBeginOptions = CallBeginSpawnNode->FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::SpawnOptionsPinName);
	UEdGraphPin* CallBeginTransform = CallBeginSpawnNode->FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::SpawnTransformPinName);
	UEdGraphPin* CallBeginReconstructPin = CallBeginSpawnNode->FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::ReconstructPinName);
	UEdGraphPin* CallBeginWorldContextPin = CallBeginSpawnNode->FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::WorldContextPinName);
	UEdGraphPin* CallBeginCollisionHandlingOverride = CallBeginSpawnNode->FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::CollisionHandlingOverridePinName);
	//
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeExec,*CallBeginExec);
	CompilerContext.MovePinLinksToIntermediate(*SpawnPoolPin,*CallBeginCharacterPoolPin);
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeOptions,*CallBeginOptions);
	CompilerContext.MovePinLinksToIntermediate(*SpawnSuccessPin,*CallBeginSuccessPin);
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeTransform,*CallBeginTransform);
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeReconstructPin,*CallBeginReconstructPin);
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeCollisionHandlingOverride,*CallBeginCollisionHandlingOverride);
	//
	if (SpawnWorldContextPin) {CompilerContext.MovePinLinksToIntermediate(*SpawnWorldContextPin,*CallBeginWorldContextPin);}
	if (SpawnNodeOwnerPin != nullptr) {CompilerContext.MovePinLinksToIntermediate(*SpawnNodeOwnerPin,*CallBeginOwnerPin);}
	//
	//
	UK2Node_CallFunction* CallFinishSpawnNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnNode,SourceGraph);
	CallFinishSpawnNode->FunctionReference.SetExternalMember(FinishSpawningFuncName,UCharacterPool::StaticClass());
	CallFinishSpawnNode->AllocateDefaultPins();
	//
	UEdGraphPin* CallFinishExec = CallFinishSpawnNode->GetExecPin();
	UEdGraphPin* CallFinishThen = CallFinishSpawnNode->GetThenPin();
	UEdGraphPin* CallFinishResult = CallFinishSpawnNode->GetReturnValuePin();
	UEdGraphPin* CallFinishCharacter = CallFinishSpawnNode->FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::CharacterPinName);
	UEdGraphPin* CallFinishTransform = CallFinishSpawnNode->FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::SpawnTransformPinName);
	//
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen,*CallFinishThen);
	CompilerContext.CopyPinLinksToIntermediate(*CallBeginTransform,*CallFinishTransform);
	//
	CallBeginResult->MakeLinkTo(CallFinishCharacter);
	CallFinishResult->PinType = SpawnNodeResult->PinType;
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult,*CallFinishResult);
	//
	//
	UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext,SourceGraph,CallBeginSpawnNode,SpawnNode,CallBeginResult,ClassToSpawn);
	LastThen->MakeLinkTo(CallFinishExec);
	//
	SpawnNode->BreakAllNodeLinks();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UK2Node_SpawnCharacterFromPool::MaybeUpdateCollisionPin(TArray<UEdGraphPin*> &OldPins) {
	for (UEdGraphPin* Pin : OldPins) {
		if (Pin->PinName == FK2Node_SpawnCharacterFromPoolHelper::NoCollisionFailPinName) {
			bool bHadOldCollisionPin = true;
			if (Pin->LinkedTo.Num() == 0) {
				bool const bOldCollisionPinValue = (Pin->DefaultValue == FString(TEXT("true")));
				UEdGraphPin* const CollisionHandlingOverridePin = GetCollisionHandlingOverridePin();
				if (CollisionHandlingOverridePin) {
					UEnum const* const MethodEnum = FindObjectChecked<UEnum>(ANY_PACKAGE, TEXT("ESpawnCharacterCollisionHandlingMethod"), true);
					CollisionHandlingOverridePin->DefaultValue =
					bOldCollisionPinValue
					? MethodEnum->GetNameStringByIndex(static_cast<int>(ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
					: MethodEnum->GetNameStringByIndex(static_cast<int>(ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding));
				}///
			} else {
				UEdGraphPin* const CollisionHandlingOverridePin = GetCollisionHandlingOverridePin();
				check(CollisionHandlingOverridePin);
				//
				UEnum* const MethodEnum = FindObjectChecked<UEnum>(ANY_PACKAGE, TEXT("ESpawnCharacterCollisionHandlingMethod"), true);
				//
				FGraphNodeCreator<UK2Node_EnumLiteral> AlwaysSpawnLiteralCreator(*GetGraph());
				UK2Node_EnumLiteral* const AlwaysSpawnLiteralNode = AlwaysSpawnLiteralCreator.CreateNode();
				AlwaysSpawnLiteralNode->Enum = MethodEnum;
				AlwaysSpawnLiteralNode->NodePosX = NodePosX;
				AlwaysSpawnLiteralNode->NodePosY = NodePosY;
				AlwaysSpawnLiteralCreator.Finalize();
				//
				FGraphNodeCreator<UK2Node_EnumLiteral> AdjustIfNecessaryLiteralCreator(*GetGraph());
				UK2Node_EnumLiteral* const AdjustIfNecessaryLiteralNode = AdjustIfNecessaryLiteralCreator.CreateNode();
				AdjustIfNecessaryLiteralNode->Enum = MethodEnum;
				AdjustIfNecessaryLiteralNode->NodePosX = NodePosX;
				AdjustIfNecessaryLiteralNode->NodePosY = NodePosY;
				AdjustIfNecessaryLiteralCreator.Finalize();
				//
				FGraphNodeCreator<UK2Node_Select> SelectCreator(*GetGraph());
				UK2Node_Select* const SelectNode = SelectCreator.CreateNode();
				SelectNode->NodePosX = NodePosX;
				SelectNode->NodePosY = NodePosY;
				SelectCreator.Finalize();
				//
				auto FindEnumInputPin = [](UK2Node_EnumLiteral const* Node) {
					for (UEdGraphPin* NodePin : Node->Pins) {
						if (NodePin->PinName == Node->GetEnumInputPinName()) {return NodePin;}
					} return (UEdGraphPin*)nullptr;
				};//
				//
				UEdGraphPin* const AlwaysSpawnLiteralNodeInputPin = FindEnumInputPin(AlwaysSpawnLiteralNode);
				UEdGraphPin* const AdjustIfNecessaryLiteralInputPin = FindEnumInputPin(AdjustIfNecessaryLiteralNode);
				//
				TArray<UEdGraphPin*> SelectOptionPins;
				SelectNode->GetOptionPins(SelectOptionPins);
				UEdGraphPin* const SelectIndexPin = SelectNode->GetIndexPin();
				//
				auto FindResultPin = [](UK2Node const* Node) {
					for (UEdGraphPin* NodePin : Node->Pins) {
						if (EEdGraphPinDirection::EGPD_Output == NodePin->Direction) {return NodePin;}
					} return (UEdGraphPin*)nullptr;
				};//
				//
				UEdGraphPin* const AlwaysSpawnLiteralNodeResultPin = FindResultPin(AlwaysSpawnLiteralNode);
				check(AlwaysSpawnLiteralNodeResultPin);
				UEdGraphPin* const AdjustIfNecessaryLiteralResultPin = FindResultPin(AdjustIfNecessaryLiteralNode);
				check(AdjustIfNecessaryLiteralResultPin);
				//
				UEdGraphPin* const OldBoolPin = Pin->LinkedTo[0];
				check(OldBoolPin);
				//
				AlwaysSpawnLiteralNodeInputPin->DefaultValue = MethodEnum->GetNameStringByIndex(static_cast<int>(ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
				AdjustIfNecessaryLiteralInputPin->DefaultValue = MethodEnum->GetNameStringByIndex(static_cast<int>(ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding));
				//
				OldBoolPin->BreakLinkTo(Pin);
				OldBoolPin->MakeLinkTo(SelectIndexPin);
				//
				AlwaysSpawnLiteralNodeResultPin->MakeLinkTo(SelectOptionPins[0]);
				AdjustIfNecessaryLiteralResultPin->MakeLinkTo(SelectOptionPins[1]);
				//
				UEdGraphPin* const SelectOutputPin = SelectNode->GetReturnValuePin();
				check(SelectOutputPin);
				SelectOutputPin->MakeLinkTo(CollisionHandlingOverridePin);
				//
				SelectNode->NotifyPinConnectionListChanged(SelectIndexPin);
				SelectNode->NotifyPinConnectionListChanged(SelectOptionPins[0]);
				SelectNode->NotifyPinConnectionListChanged(SelectOptionPins[1]);
				SelectNode->NotifyPinConnectionListChanged(SelectOutputPin);
			}///
		}///
	}///
}

void UK2Node_SpawnCharacterFromPool::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*> &OldPins) {
	AllocateDefaultPins();
	//
	UClass* UseSpawnClass = GetClassToSpawn(&OldPins);
	if( UseSpawnClass != nullptr) {
		TArray<UEdGraphPin*> ClassPins;
		CreatePinsForClass(UseSpawnClass,&ClassPins);
	}///
	//
	MaybeUpdateCollisionPin(OldPins);
	RestoreSplitPins(OldPins);
}

bool UK2Node_SpawnCharacterFromPool::IsSpawnVarPin(UEdGraphPin* Pin) {
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	//
	UEdGraphPin* ParentPin = Pin->ParentPin;
	while (ParentPin) {
		if (ParentPin->PinName == FK2Node_SpawnCharacterFromPoolHelper::SpawnTransformPinName) {return false;}
	ParentPin = ParentPin->ParentPin;}
	//
	return (
		Pin->PinName != K2Schema->PN_Execute &&
		Pin->PinName != K2Schema->PN_Then &&
		Pin->PinName != K2Schema->PN_ReturnValue &&
		Pin->PinName != FK2Node_SpawnCharacterFromPoolHelper::PoolPinName &&
		Pin->PinName != FK2Node_SpawnCharacterFromPoolHelper::OwnerPinName &&
		Pin->PinName != FK2Node_SpawnCharacterFromPoolHelper::SuccessPinName &&
		Pin->PinName != FK2Node_SpawnCharacterFromPoolHelper::ReconstructPinName &&
		Pin->PinName != FK2Node_SpawnCharacterFromPoolHelper::WorldContextPinName &&
		Pin->PinName != FK2Node_SpawnCharacterFromPoolHelper::SpawnOptionsPinName &&
		Pin->PinName != FK2Node_SpawnCharacterFromPoolHelper::SpawnTransformPinName &&
		Pin->PinName != FK2Node_SpawnCharacterFromPoolHelper::CollisionHandlingOverridePinName
	);//
}

void UK2Node_SpawnCharacterFromPool::PostPlacedNewNode() {
	Super::PostPlacedNewNode();

	UClass* UseSpawnClass = GetClassToSpawn();
	if (UseSpawnClass != nullptr) {
		TArray<UEdGraphPin*> ClassPins;
		CreatePinsForClass(UseSpawnClass,&ClassPins);
	}///
}

void UK2Node_SpawnCharacterFromPool::OnClassPinChanged() {
 	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	//
	TArray<UEdGraphPin*> OldPins = Pins;
	TArray<UEdGraphPin*> OldClassPins;
	//
	for (int32 i = 0; i < OldPins.Num(); i++) {
		UEdGraphPin* OldPin = OldPins[i];
		if (IsSpawnVarPin(OldPin)) {
			Pins.Remove(OldPin);
			OldClassPins.Add(OldPin);
		}///
	}///
	//
	CachedNodeTitle.MarkDirty();
	//
	TArray<UEdGraphPin*> NewClassPins;
	UClass* UseSpawnClass = GetClassToSpawn();
	if (UseSpawnClass != nullptr) {CreatePinsForClass(UseSpawnClass,&NewClassPins);}
	//
	UEdGraphPin* ResultPin = GetResultPin();
	TArray<UEdGraphPin*> ResultPinConnectionList = ResultPin->LinkedTo;
	ResultPin->BreakAllPinLinks();
	//
	for (UEdGraphPin* Connections : ResultPinConnectionList) {
		K2Schema->TryCreateConnection(ResultPin, Connections);
	}///
	//
	RewireOldPinsToNewPins(OldClassPins, NewClassPins);
	DestroyPinList(OldClassPins);
	//
	UEdGraph* Graph = GetGraph();
	Graph->NotifyGraphChanged();
	//
	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

void UK2Node_SpawnCharacterFromPool::PinConnectionListChanged(UEdGraphPin* ChangedPin) {
	if (ChangedPin && (ChangedPin->PinName==FK2Node_SpawnCharacterFromPoolHelper::PoolPinName)) {OnClassPinChanged();}
}

void UK2Node_SpawnCharacterFromPool::PinDefaultValueChanged(UEdGraphPin* ChangedPin) {
	if (ChangedPin && (ChangedPin->PinName==FK2Node_SpawnCharacterFromPoolHelper::PoolPinName)) {OnClassPinChanged();}
}

UEdGraphPin* UK2Node_SpawnCharacterFromPool::GetThenPin() const {
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	//
	UEdGraphPin* Pin = FindPinChecked(K2Schema->PN_Then);
	check(Pin->Direction==EGPD_Output); return Pin;
}

UEdGraphPin* UK2Node_SpawnCharacterFromPool::GetPoolPin(const TArray<UEdGraphPin*>* InPinsToSearch) const {
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;
	//
	UEdGraphPin* Pin = nullptr;
	for( auto PinIt = PinsToSearch->CreateConstIterator(); PinIt; ++PinIt ) {
		UEdGraphPin* TestPin = *PinIt;
		if (TestPin && TestPin->PinName == FK2Node_SpawnCharacterFromPoolHelper::PoolPinName) {Pin = TestPin; break;}
	}///
	//
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SpawnCharacterFromPool::GetSpawnOptionsPin() const {
	UEdGraphPin* Pin = FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::SpawnOptionsPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SpawnCharacterFromPool::GetSpawnTransformPin() const {
	UEdGraphPin* Pin = FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::SpawnTransformPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SpawnCharacterFromPool::GetCollisionHandlingOverridePin() const {
	UEdGraphPin* const Pin = FindPinChecked(FK2Node_SpawnCharacterFromPoolHelper::CollisionHandlingOverridePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SpawnCharacterFromPool::GetOwnerPin() const {
	UEdGraphPin* Pin = FindPin(FK2Node_SpawnCharacterFromPoolHelper::OwnerPinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SpawnCharacterFromPool::GetReconstructPin() const {
	UEdGraphPin* Pin = FindPin(FK2Node_SpawnCharacterFromPoolHelper::ReconstructPinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Input); return Pin;
}

UEdGraphPin* UK2Node_SpawnCharacterFromPool::GetWorldContextPin() const {
	UEdGraphPin* Pin = FindPin(FK2Node_SpawnCharacterFromPoolHelper::WorldContextPinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SpawnCharacterFromPool::GetSuccessPin() const {
	UEdGraphPin* Pin = FindPin(FK2Node_SpawnCharacterFromPoolHelper::SuccessPinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_SpawnCharacterFromPool::GetResultPin() const {
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	UEdGraphPin* Pin = FindPinChecked(K2Schema->PN_ReturnValue);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

FLinearColor UK2Node_SpawnCharacterFromPool::GetNodeTitleColor() const {
	return Super::GetNodeTitleColor();
}

bool UK2Node_SpawnCharacterFromPool::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const  {
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
	return Super::IsCompatibleWithGraph(TargetGraph) && (!Blueprint || (FBlueprintEditorUtils::FindUserConstructionScript(Blueprint) != TargetGraph && Blueprint->GeneratedClass->GetDefaultObject()->ImplementsGetWorld()));
}

void UK2Node_SpawnCharacterFromPool::GetNodeAttributes( TArray<TKeyValuePair<FString, FString>> &OutNodeAttributes ) const {
	UClass* ClassToSpawn = GetClassToSpawn();
	const FString ClassToSpawnStr = ClassToSpawn ? ClassToSpawn->GetName() : TEXT("InvalidClass");
	//
	OutNodeAttributes.Add(TKeyValuePair<FString,FString>(TEXT("Type"),TEXT("SpawnCharacterFromPool")));
	OutNodeAttributes.Add(TKeyValuePair<FString,FString>(TEXT("Class"),GetClass()->GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString,FString>(TEXT("Name"),GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString,FString>(TEXT("CharacterClass"),ClassToSpawnStr));
}

FNodeHandlingFunctor* UK2Node_SpawnCharacterFromPool::CreateNodeHandler(FKismetCompilerContext &CompilerContext) const {
	return new FNodeHandlingFunctor(CompilerContext);
}

bool UK2Node_SpawnCharacterFromPool::HasExternalDependencies(TArray<class UStruct*>* OptionalOutput) const {
	UClass* SourceClass = GetClassToSpawn();
	const UBlueprint* SourceBlueprint = GetBlueprint();
	const bool bResult = (SourceClass != nullptr) && (SourceClass->ClassGeneratedBy != SourceBlueprint);
	//
	if (bResult && OptionalOutput) {OptionalOutput->AddUnique(SourceClass);}
	//
	const bool bSuperResult = Super::HasExternalDependencies(OptionalOutput);
	return bSuperResult || bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UK2Node_SpawnCharacterFromPool::GetPinHoverText(const UEdGraphPin &Pin, FString &HoverTextOut) const {
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	//
	if (UEdGraphPin* PoolPin = GetPoolPin()) {
		K2Schema->ConstructBasicPinTooltip(*PoolPin,LOCTEXT("PoolPinDescription","The Object-Pool Owner of Character Instance."),PoolPin->PinToolTip);
	} if (UEdGraphPin* TransformPin = GetSpawnTransformPin()) {
		K2Schema->ConstructBasicPinTooltip(*TransformPin,LOCTEXT("TransformPinDescription","The transform to spawn the Character with"),TransformPin->PinToolTip);
	} if (UEdGraphPin* CollisionHandlingOverridePin = GetCollisionHandlingOverridePin()) {
		K2Schema->ConstructBasicPinTooltip(*CollisionHandlingOverridePin,LOCTEXT("CollisionHandlingOverridePinDescription","Specifies how to handle collisions at the Spawn Point. If undefined,uses Character Class Settings."),CollisionHandlingOverridePin->PinToolTip);
	} if (UEdGraphPin* OwnerPin = GetOwnerPin()) {
		K2Schema->ConstructBasicPinTooltip(*OwnerPin,LOCTEXT("OwnerPinDescription","Can be left empty; primarily used for replication or visibility."),OwnerPin->PinToolTip);
	} if (UEdGraphPin* ReconstructPin = GetReconstructPin()) {
		K2Schema->ConstructBasicPinTooltip(*ReconstructPin,LOCTEXT("ReconstructPinDescription","If checked, this will force the Spawning Character to re-run its Construction Scripts;\nThis results on expensive respawn operation and will affect performance!\nAvoid pulling Characters every second with this option enabled."),ReconstructPin->PinToolTip);
	} if (UEdGraphPin* ResultPin = GetResultPin()) {
		K2Schema->ConstructBasicPinTooltip(*ResultPin,LOCTEXT("ResultPinDescription","The Character Class Spawned from the Pool."),ResultPin->PinToolTip);
	} return Super::GetPinHoverText(Pin,HoverTextOut);
}

FText UK2Node_SpawnCharacterFromPool::GetTooltipText() const {
	return NodeTooltip;
}

FSlateIcon UK2Node_SpawnCharacterFromPool::GetIconAndTint(FLinearColor &OutColor) const {
	static FSlateIcon Icon(TEXT("OBJPoolStyle"),TEXT("ClassIcon.ObjectPool"));
	return Icon;
}

void UK2Node_SpawnCharacterFromPool::GetMenuActions(FBlueprintActionDatabaseRegistrar &ActionRegistrar) const {
	UClass* ActionKey = GetClass();
	//
	if (ActionRegistrar.IsOpenForRegistration(ActionKey)) {
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}///
}

FText UK2Node_SpawnCharacterFromPool::GetMenuCategory() const {
	return LOCTEXT("PoolCategory","Object Pool");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////