// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include "AdaptiveActionParserRegistration.h"
#include "AdaptiveActionSet.h"
#include "AdaptiveColumn.h"
#include "AdaptiveColumnSet.h"
#include "AdaptiveContainer.h"
#include "AdaptiveChoiceInput.h"
#include "AdaptiveChoiceSetInput.h"
#include "AdaptiveDateInput.h"
#include "AdaptiveElementParserRegistration.h"
#include "AdaptiveExecuteAction.h"
#include "AdaptiveFact.h"
#include "AdaptiveFactSet.h"
#include "AdaptiveImage.h"
#include "AdaptiveImageSet.h"
#include "AdaptiveMedia.h"
#include "AdaptiveMediaSource.h"
#include "AdaptiveNumberInput.h"
#include "AdaptiveOpenUrlAction.h"
#include "AdaptiveRequirement.h"
#include "AdaptiveRichTextBlock.h"
#include "AdaptiveShowCardAction.h"
#include "AdaptiveSubmitAction.h"
#include "AdaptiveTable.h"
#include "AdaptiveTextBlock.h"
#include "AdaptiveTextInput.h"
#include "AdaptiveTextRun.h"
#include "AdaptiveTimeInput.h"
#include "AdaptiveToggleInput.h"
#include "AdaptiveToggleVisibilityTarget.h"
#include "AdaptiveToggleVisibilityAction.h"
#include "AdaptiveUnsupportedAction.h"
#include "AdaptiveUnsupportedElement.h"
#include "AdaptiveWarning.h"
#include "CustomActionWrapper.h"
#include "CustomElementWrapper.h"

using namespace AdaptiveCards;
using namespace Microsoft::WRL;
using namespace ABI::Windows::Data::Json;
using namespace Microsoft::WRL;
using namespace AdaptiveCards::ObjectModel::WinAppSDK;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;

std::string WStringToString(std::wstring_view in)
{
    const int length_in = static_cast<int>(in.length());

    if (length_in > 0)
    {
        const int length_out = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, in.data(), length_in, NULL, 0, NULL, NULL);

        if (length_out > 0)
        {
            std::string out(length_out, '\0');

            const int length_written =
                ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, in.data(), length_in, out.data(), length_out, NULL, NULL);

            if (length_written == length_out)
            {
                return out;
            }
        }

        throw bad_string_conversion();
    }

    return {};
}

std::wstring StringToWString(std::string_view in)
{
    const int length_in = static_cast<int>(in.length());

    if (length_in > 0)
    {
        const int length_out = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, in.data(), length_in, NULL, 0);

        if (length_out > 0)
        {
            std::wstring out(length_out, L'\0');

            const int length_written =
                ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, in.data(), length_in, out.data(), length_out);

            if (length_written == length_out)
            {
                return out;
            }
        }

        throw bad_string_conversion();
    }

    return {};
}

HRESULT UTF8ToHString(std::string_view in, _Outptr_ HSTRING* out) noexcept
try
{
    if (out == nullptr)
    {
        return E_INVALIDARG;
    }
    else
    {
        std::wstring wide = StringToWString(in);
        return WindowsCreateString(wide.c_str(), static_cast<UINT32>(wide.length()), out);
    }
}
CATCH_RETURN;

HRESULT HStringToUTF8(HSTRING in, std::string& out) noexcept
try
{
    UINT32 length = 0U;
    const auto* ptr_wide = WindowsGetStringRawBuffer(in, &length);
    out = WStringToString(std::wstring_view(ptr_wide, length));

    return S_OK;
}
CATCH_RETURN;

std::string HStringToUTF8(HSTRING in)
{
    std::string typeAsKey;
    if (SUCCEEDED(HStringToUTF8(in, typeAsKey)))
    {
        return typeAsKey;
    }

    return {};
}

template<typename TSharedBaseType, typename TAdaptiveBaseType, typename TAdaptiveType>
std::shared_ptr<TSharedBaseType> GetSharedModel(_In_ TAdaptiveBaseType* item)
{
    ComPtr<TAdaptiveType> adaptiveElement = PeekInnards<TAdaptiveType>(item);

    std::shared_ptr<TSharedBaseType> sharedModelElement;
    if (adaptiveElement && SUCCEEDED(adaptiveElement->GetSharedModel(sharedModelElement)))
    {
        return sharedModelElement;
    }
    else
    {
        return nullptr;
    }
}

HRESULT GenerateSharedElement(_In_ ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement* item,
                              std::shared_ptr<AdaptiveCards::BaseCardElement>& baseCardElement)
{
    ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType elementType;
    RETURN_IF_FAILED(item->get_ElementType(&elementType));

    switch (elementType)
    {
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::ActionSet:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveActionSet>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::ChoiceSetInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveChoiceSetInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::Column:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveColumn>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::ColumnSet:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveColumnSet>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::Container:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveContainer>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::DateInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveDateInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::FactSet:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveFactSet>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::Image:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveImage>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::ImageSet:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveImageSet>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::NumberInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveNumberInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::Media:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveMedia>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::RichTextBlock:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveRichTextBlock>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::TextBlock:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTextBlock>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::TextInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTextInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::TimeInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTimeInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::ToggleInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveToggleInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::Table:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTable>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::Unsupported:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveUnsupportedElement>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ElementType::Custom:
        baseCardElement = std::make_shared<CustomElementWrapper>(item);
        break;
    }

    if (baseCardElement == nullptr)
    {
        return E_INVALIDARG;
    }

    return S_OK;
}

HRESULT GenerateSharedElements(_In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement*>* items,
                               std::vector<std::shared_ptr<AdaptiveCards::BaseCardElement>>& containedElements)
{
    containedElements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement>(
        items, [&](ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement* item) {
            std::shared_ptr<AdaptiveCards::BaseCardElement> baseCardElement;
            RETURN_IF_FAILED(GenerateSharedElement(item, baseCardElement));
            containedElements.push_back(std::move(baseCardElement));

            return S_OK;
        });

    return S_OK;
}

HRESULT GenerateSharedAction(_In_ ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement* action,
                             std::shared_ptr<AdaptiveCards::BaseActionElement>& sharedAction)
{
    ABI::AdaptiveCards::ObjectModel::WinAppSDK::ActionType actionType;
    RETURN_IF_FAILED(action->get_ActionType(&actionType));

    switch (actionType)
    {
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ActionType::OpenUrl:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveOpenUrlAction>(
                action);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ActionType::ShowCard:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveShowCardAction>(
                action);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ActionType::Submit:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveSubmitAction>(
                action);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ActionType::ToggleVisibility:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveToggleVisibilityAction>(
                action);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ActionType::Execute:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveExecuteAction>(
                action);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ActionType::Custom:
        sharedAction = std::make_shared<CustomActionWrapper>(action);
        break;
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::ActionType::Unsupported:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement, AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveUnsupportedAction>(
                action);
    }

    return S_OK;
}

HRESULT GenerateSharedActions(_In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement*>* actions,
                              std::vector<std::shared_ptr<AdaptiveCards::BaseActionElement>>& containedElements)
{
    containedElements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement>(
        actions, [&](ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement* action) {
            std::shared_ptr<AdaptiveCards::BaseActionElement> baseActionElement;
            GenerateSharedAction(action, baseActionElement);
            containedElements.push_back(baseActionElement);
            return S_OK;
        });

    return S_OK;
}

HRESULT GenerateSharedRequirements(
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveRequirement*>* adaptiveRequirements,
    std::unordered_map<std::string, AdaptiveCards::SemanticVersion>& sharedRequirements) noexcept
try
{
    sharedRequirements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveRequirement, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveRequirement>(
        adaptiveRequirements, [&](ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveRequirement* requirement) {
            HString nameHString;
            RETURN_IF_FAILED(requirement->get_Name(nameHString.GetAddressOf()));

            HString versionHString;
            RETURN_IF_FAILED(requirement->get_Version(versionHString.GetAddressOf()));

            std::string nameString;
            RETURN_IF_FAILED(HStringToUTF8(nameHString.Get(), nameString));

            std::string versionString;
            RETURN_IF_FAILED(HStringToUTF8(versionHString.Get(), versionString));

            if (versionString == "*")
            {
                sharedRequirements.emplace(std::move(nameString), "0");
            }
            else
            {
                sharedRequirements.emplace(std::move(nameString), std::move(versionString));
            }

            return S_OK;
        });

    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateSharedInlines(ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveInline*>* inlines,
                              std::vector<std::shared_ptr<AdaptiveCards::Inline>>& containedElements)
{
    containedElements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveInline, ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveInline>(
        inlines, [&](ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveInline* thisInline) {
            // We only support text runs for now
            ComPtr<AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTextRun> adaptiveTextRun =
                PeekInnards<AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTextRun>(thisInline);
            if (adaptiveTextRun == nullptr)
            {
                return E_INVALIDARG;
            }

            std::shared_ptr<AdaptiveCards::TextRun> sharedTextRun;
            RETURN_IF_FAILED(adaptiveTextRun->GetSharedModel(sharedTextRun));
            containedElements.push_back(std::AdaptivePointerCast<AdaptiveCards::Inline>(sharedTextRun));
            return S_OK;
        });

    return S_OK;
}

HRESULT GenerateElementProjection(_In_ const std::shared_ptr<AdaptiveCards::BaseCardElement>& baseElement,
                                  _COM_Outptr_ ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement** projectedElement) noexcept
try
{
    *projectedElement = nullptr;
    switch (baseElement->GetElementType())
    {
    case CardElementType::TextBlock:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTextBlock>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::TextBlock>(baseElement)));
        break;
    case CardElementType::Image:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveImage>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::Image>(baseElement)));
        break;
    case CardElementType::Container:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveContainer>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::Container>(baseElement)));
        break;
    case CardElementType::ColumnSet:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveColumnSet>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::ColumnSet>(baseElement)));
        break;
    case CardElementType::FactSet:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveFactSet>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::FactSet>(baseElement)));
        break;
    case CardElementType::ImageSet:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveImageSet>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::ImageSet>(baseElement)));
        break;
    case CardElementType::ChoiceSetInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveChoiceSetInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::ChoiceSetInput>(baseElement)));
        break;
    case CardElementType::DateInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveDateInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::DateInput>(baseElement)));
        break;
    case CardElementType::Media:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveMedia>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::Media>(baseElement)));
        break;
    case CardElementType::NumberInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveNumberInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::NumberInput>(baseElement)));
        break;
    case CardElementType::TextInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTextInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::TextInput>(baseElement)));
        break;
    case CardElementType::TimeInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTimeInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::TimeInput>(baseElement)));
        break;
    case CardElementType::ToggleInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveToggleInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::ToggleInput>(baseElement)));
        break;
    case CardElementType::ActionSet:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveActionSet>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::ActionSet>(baseElement)));
        break;
    case CardElementType::RichTextBlock:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveRichTextBlock>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::RichTextBlock>(baseElement)));
        break;
    case CardElementType::Column:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveColumn>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::Column>(baseElement)));
        break;
    case CardElementType::Table:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTable>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::Table>(baseElement)));
        break;
    case CardElementType::Custom:
        RETURN_IF_FAILED(
            std::AdaptivePointerCast<::AdaptiveCards::ObjectModel::WinAppSDK::CustomElementWrapper>(baseElement)->GetWrappedElement(projectedElement));
        break;
    case CardElementType::Unknown:
    default:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveUnsupportedElement>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::UnknownElement>(baseElement)));
        break;
    }

    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateContainedElementsProjection(
    const std::vector<std::shared_ptr<AdaptiveCards::BaseCardElement>>& containedElements,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement*>* projectedParentContainer) noexcept
try
{
    for (auto& containedElement : containedElements)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveCardElement> projectedContainedElement;
        GenerateElementProjection(containedElement, &projectedContainedElement);
        if (projectedContainedElement != nullptr)
        {
            RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedElement.Detach()));
        }
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateActionsProjection(
    const std::vector<std::shared_ptr<AdaptiveCards::BaseActionElement>>& containedActions,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement*>* projectedParentContainer) noexcept
try
{
    for (auto& containedAction : containedActions)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement> projectedContainedAction;
        RETURN_IF_FAILED(GenerateActionProjection(containedAction, &projectedContainedAction));

        RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedAction.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateActionProjection(const std::shared_ptr<AdaptiveCards::BaseActionElement>& action,
                                 _COM_Outptr_ ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionElement** projectedAction) noexcept
try
{
    if (action == nullptr)
    {
        *projectedAction = nullptr;
        return S_OK;
    }

    switch (action->GetElementType())
    {
    case ActionType::OpenUrl:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveOpenUrlAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::OpenUrlAction>(action)));
        break;
    case ActionType::ShowCard:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveShowCardAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::ShowCardAction>(action)));
        break;
    case ActionType::Submit:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveSubmitAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::SubmitAction>(action)));
        break;
    case ActionType::ToggleVisibility:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveToggleVisibilityAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::ToggleVisibilityAction>(action)));
        break;
    case ActionType::Execute:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveExecuteAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::ExecuteAction>(action)));
        break;
    case ActionType::Custom:
        RETURN_IF_FAILED(std::AdaptivePointerCast<CustomActionWrapper>(action)->GetWrappedElement(projectedAction));
        break;
    case ActionType::UnknownAction:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveUnsupportedAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::UnknownAction>(action)));
        break;
    default:
        return E_UNEXPECTED;
        break;
    }

    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateInlinesProjection(const std::vector<std::shared_ptr<AdaptiveCards::Inline>>& containedElements,
                                  ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveInline*>* projectedParentContainer) noexcept
try
{
    for (auto& containedElement : containedElements)
    {
        // Only support text runs for now
        if (containedElement->GetInlineType() != InlineElementType::TextRun)
        {
            return E_NOTIMPL;
        }

        ComPtr<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveInline> projectedContainedElement;
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveTextRun>(
            &projectedContainedElement, std::static_pointer_cast<AdaptiveCards::TextRun>(containedElement)));

        RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedElement.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateRequirementsProjection(
    const std::unordered_map<std::string, SemanticVersion>& sharedRequirements,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveRequirement*>* projectedRequirementVector) noexcept
try
{
    for (const auto& sharedRequirement : sharedRequirements)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveRequirement> projectedRequirement;
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveRequirement>(&projectedRequirement,
                                                                                                   sharedRequirement));
        RETURN_IF_FAILED(projectedRequirementVector->Append(projectedRequirement.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT StringToJsonObject(const std::string& inputString, _COM_Outptr_ IJsonObject** result)
{
    HString asHstring;
    RETURN_IF_FAILED(UTF8ToHString(inputString, asHstring.GetAddressOf()));
    return HStringToJsonObject(asHstring.Get(), result);
}

HRESULT HStringToJsonObject(const HSTRING& inputHString, _COM_Outptr_ IJsonObject** result)
{
    ComPtr<IJsonObjectStatics> jObjectStatics;
    RETURN_IF_FAILED(GetActivationFactory(HStringReference(RuntimeClass_Windows_Data_Json_JsonObject).Get(), &jObjectStatics));
    ComPtr<IJsonObject> jObject;
    HRESULT hr = jObjectStatics->Parse(inputHString, &jObject);
    if (FAILED(hr))
    {
        RETURN_IF_FAILED(ActivateInstance(HStringReference(RuntimeClass_Windows_Data_Json_JsonObject).Get(), &jObject));
    }
    *result = jObject.Detach();
    return S_OK;
}

HRESULT JsonObjectToString(_In_ IJsonObject* inputJson, std::string& result)
{
    HString asHstring;
    RETURN_IF_FAILED(JsonObjectToHString(inputJson, asHstring.GetAddressOf()));
    return HStringToUTF8(asHstring.Get(), result);
}

HRESULT JsonObjectToHString(_In_ IJsonObject* inputJson, _Outptr_ HSTRING* result)
{
    if (!inputJson)
    {
        return E_INVALIDARG;
    }
    ComPtr<IJsonObject> localInputJson(inputJson);
    ComPtr<IJsonValue> asJsonValue;
    RETURN_IF_FAILED(localInputJson.As(&asJsonValue));
    return (asJsonValue->Stringify(result));
}

HRESULT StringToJsonValue(const std::string inputString, _COM_Outptr_ IJsonValue** result)
{
    HString asHstring;
    RETURN_IF_FAILED(UTF8ToHString(inputString, asHstring.GetAddressOf()));
    return HStringToJsonValue(asHstring.Get(), result);
}

HRESULT HStringToJsonValue(const HSTRING& inputHString, _COM_Outptr_ IJsonValue** result)
{
    ComPtr<IJsonValueStatics> jValueStatics;
    RETURN_IF_FAILED(GetActivationFactory(HStringReference(RuntimeClass_Windows_Data_Json_JsonValue).Get(), &jValueStatics));
    ComPtr<IJsonValue> jValue;
    HRESULT hr = jValueStatics->Parse(inputHString, &jValue);
    if (FAILED(hr))
    {
        RETURN_IF_FAILED(ActivateInstance(HStringReference(RuntimeClass_Windows_Data_Json_JsonValue).Get(), &jValue));
    }
    *result = jValue.Detach();
    return S_OK;
}

HRESULT JsonValueToString(_In_ IJsonValue* inputValue, std::string& result)
{
    HString asHstring;
    RETURN_IF_FAILED(JsonValueToHString(inputValue, asHstring.GetAddressOf()));
    return HStringToUTF8(asHstring.Get(), result);
}

HRESULT JsonValueToHString(_In_ IJsonValue* inputJsonValue, _Outptr_ HSTRING* result)
{
    if (!inputJsonValue)
    {
        return E_INVALIDARG;
    }
    ComPtr<IJsonValue> localInputJsonValue(inputJsonValue);
    return (localInputJsonValue->Stringify(result));
}

HRESULT JsonCppToJsonObject(const Json::Value& jsonCppValue, _COM_Outptr_ IJsonObject** result)
{
    std::string jsonString = ParseUtil::JsonToString(jsonCppValue);
    return StringToJsonObject(jsonString, result);
}

HRESULT JsonObjectToJsonCpp(_In_ ABI::Windows::Data::Json::IJsonObject* jsonObject, _Out_ Json::Value* jsonCppValue)
{
    std::string jsonString;
    RETURN_IF_FAILED(JsonObjectToString(jsonObject, jsonString));

    *jsonCppValue = ParseUtil::GetJsonValueFromString(jsonString);

    return S_OK;
}


void RemoteResourceElementToRemoteResourceInformationVector(_In_ ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveElementWithRemoteResources* remoteResourceElement,
                                                            std::vector<RemoteResourceInformation>& resourceUris)
{
    ComPtr<ABI::Windows::Foundation::Collections::IVectorView<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveRemoteResourceInformation*>> remoteResources;
    THROW_IF_FAILED(remoteResourceElement->GetResourceInformation(remoteResources.GetAddressOf()));

    ComPtr<IIterable<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveRemoteResourceInformation*>> vectorIterable;
    THROW_IF_FAILED(remoteResources.As<IIterable<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveRemoteResourceInformation*>>(&vectorIterable));

    Microsoft::WRL::ComPtr<IIterator<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveRemoteResourceInformation*>> vectorIterator;
    HRESULT hr = vectorIterable->First(&vectorIterator);

    boolean hasCurrent;
    THROW_IF_FAILED(vectorIterator->get_HasCurrent(&hasCurrent));

    while (SUCCEEDED(hr) && hasCurrent)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveRemoteResourceInformation> resourceInformation;
        THROW_IF_FAILED(vectorIterator->get_Current(&resourceInformation));

        HString url;
        THROW_IF_FAILED(resourceInformation->get_Url(url.GetAddressOf()));

        RemoteResourceInformation uriInfo;
        THROW_IF_FAILED(HStringToUTF8(url.Get(), uriInfo.url));

        HString mimeType;
        THROW_IF_FAILED(resourceInformation->get_MimeType(mimeType.GetAddressOf()));

        uriInfo.mimeType = HStringToUTF8(mimeType.Get());

        resourceUris.push_back(uriInfo);

        hr = vectorIterator->MoveNext(&hasCurrent);
    }
}

HRESULT SharedWarningsToAdaptiveWarnings(
    const std::vector<std::shared_ptr<AdaptiveCardParseWarning>>& sharedWarnings,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveWarning*>* adaptiveWarnings)
{
    for (const auto& sharedWarning : sharedWarnings)
    {
        HString warningMessage;
        RETURN_IF_FAILED(UTF8ToHString(sharedWarning->GetReason(), warningMessage.GetAddressOf()));

        ABI::AdaptiveCards::ObjectModel::WinAppSDK::WarningStatusCode statusCode =
            static_cast<ABI::AdaptiveCards::ObjectModel::WinAppSDK::WarningStatusCode>(sharedWarning->GetStatusCode());

        ComPtr<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveWarning> adaptiveWarning;
        RETURN_IF_FAILED(MakeAndInitialize<AdaptiveWarning>(&adaptiveWarning, statusCode, warningMessage.Get()));

        RETURN_IF_FAILED(adaptiveWarnings->Append(adaptiveWarning.Get()));
    }

    return S_OK;
}

HRESULT AdaptiveWarningsToSharedWarnings(
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveWarning*>* adaptiveWarnings,
    std::vector<std::shared_ptr<AdaptiveCardParseWarning>>& sharedWarnings)
{
    ComPtr<ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveWarning*>> localAdaptiveWarnings{
        adaptiveWarnings};
    ComPtr<IIterable<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveWarning*>> vectorIterable;
    RETURN_IF_FAILED(localAdaptiveWarnings.As<IIterable<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveWarning*>>(&vectorIterable));

    Microsoft::WRL::ComPtr<IIterator<ABI::AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveWarning*>> vectorIterator;
    HRESULT hr = vectorIterable->First(&vectorIterator);

    boolean hasCurrent;
    THROW_IF_FAILED(vectorIterator->get_HasCurrent(&hasCurrent));

    while (SUCCEEDED(hr) && hasCurrent)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveWarning> adaptiveWarning;
        RETURN_IF_FAILED(vectorIterator->get_Current(&adaptiveWarning));

        HString message;
        RETURN_IF_FAILED(adaptiveWarning->get_Message(message.GetAddressOf()));

        ABI::AdaptiveCards::ObjectModel::WinAppSDK::WarningStatusCode statusCode;
        RETURN_IF_FAILED(adaptiveWarning->get_StatusCode(&statusCode));

        sharedWarnings.push_back(std::make_shared<AdaptiveCardParseWarning>(static_cast<AdaptiveCards::WarningStatusCode>(statusCode),
                                                                            HStringToUTF8(message.Get())));

        hr = vectorIterator->MoveNext(&hasCurrent);
    }

    return S_OK;
}

ABI::AdaptiveCards::ObjectModel::WinAppSDK::FallbackType MapSharedFallbackTypeToUwp(const AdaptiveCards::FallbackType type)
{
    switch (type)
    {
    case FallbackType::Drop:
    {
        return ABI::AdaptiveCards::ObjectModel::WinAppSDK::FallbackType::Drop;
    }

    case FallbackType::Content:
    {
        return ABI::AdaptiveCards::ObjectModel::WinAppSDK::FallbackType::Content;
    }

    case FallbackType::None:
    default:
    {
        return ABI::AdaptiveCards::ObjectModel::WinAppSDK::FallbackType::None;
    }
    }
}

AdaptiveCards::FallbackType MapUwpFallbackTypeToShared(const ABI::AdaptiveCards::ObjectModel::WinAppSDK::FallbackType type)
{
    switch (type)
    {
    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::FallbackType::Drop:
    {
        return FallbackType::Drop;
    }

    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::FallbackType::Content:
    {
        return FallbackType::Content;
    }

    case ABI::AdaptiveCards::ObjectModel::WinAppSDK::FallbackType::None:
    default:
    {
        return FallbackType::None;
    }
    }
}

HRESULT GetAdaptiveActionParserRegistrationFromSharedModel(
    const std::shared_ptr<ActionParserRegistration>& sharedActionParserRegistration,
    _COM_Outptr_ ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveActionParserRegistration** adaptiveActionParserRegistration)
{
    // Look up the well known action parser registration to see if we've got a custom action registration to pass
    std::shared_ptr<ActionElementParser> sharedActionParser =
        sharedActionParserRegistration->GetParser(c_upwActionParserRegistration);

    if (sharedActionParser != nullptr)
    {
        // The shared model wraps the passed in parsers. Get our SharedModelActionParser from it so we can retrieve the
        // IAdaptiveActionParserRegistration
        std::shared_ptr<ActionElementParserWrapper> parserWrapper =
            std::static_pointer_cast<ActionElementParserWrapper>(sharedActionParser);

        std::shared_ptr<SharedModelActionParser> sharedModelParser =
            std::static_pointer_cast<SharedModelActionParser>(parserWrapper->GetActualParser());

        RETURN_IF_FAILED(sharedModelParser->GetAdaptiveParserRegistration(adaptiveActionParserRegistration));
    }
    else
    {
        RETURN_IF_FAILED(MakeAndInitialize<AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveActionParserRegistration>(adaptiveActionParserRegistration));
    }

    return S_OK;
}

HRESULT GetAdaptiveElementParserRegistrationFromSharedModel(
    const std::shared_ptr<ElementParserRegistration>& sharedElementParserRegistration,
    _COM_Outptr_ ABI::AdaptiveCards::ObjectModel::WinAppSDK::IAdaptiveElementParserRegistration** adaptiveElementParserRegistration)
{
    // Look up the well known Element parser registration to see if we've got a custom Element registration to pass
    std::shared_ptr<BaseCardElementParser> sharedElementParser =
        sharedElementParserRegistration->GetParser(c_uwpElementParserRegistration);

    if (sharedElementParser != nullptr)
    {
        // The shared model wraps the passed in parsers. Get our SharedModelElementParser from it so we can retrieve the
        // IAdaptiveElementParserRegistration
        std::shared_ptr<BaseCardElementParserWrapper> parserWrapper =
            std::static_pointer_cast<BaseCardElementParserWrapper>(sharedElementParser);

        std::shared_ptr<SharedModelElementParser> sharedModelParser =
            std::static_pointer_cast<SharedModelElementParser>(parserWrapper->GetActualParser());

        RETURN_IF_FAILED(sharedModelParser->GetAdaptiveParserRegistration(adaptiveElementParserRegistration));
    }
    else
    {
        RETURN_IF_FAILED(MakeAndInitialize<AdaptiveCards::ObjectModel::WinAppSDK::AdaptiveElementParserRegistration>(
            adaptiveElementParserRegistration));
    }

    return S_OK;
}