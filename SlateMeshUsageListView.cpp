#include "SlateMeshUsageListView.h"

#include "EditorAssetLibrary.h"
#include "Toolset/SceneTool.h"

#include "EngineUtils.h"
#include "JAssetLog.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Widgets/Input/SSearchBox.h"

// ==================================================================================
// ==================================================================================

TSharedRef<SWidget> SMeshUsageItemView::GenerateWidgetForColumn(const FName& ColumnName)
{
	auto _Item = ItemData.Pin(); //解包弱指针（拿钥匙开门） ItemData 是一个弱指针（TWeakPtr）。为了安全地读取里面的数据，必须先调用 .Pin() 方法。
														//.Pin() 会尝试将它升级为强智能指针 TSharedPtr。如果升级成功（指向的数据还活着），我们就能通过 _Item-> 去访问里面的具体数据
	
	if (ColumnName == TEXT("Num"))  //渲染“Num” 列  
	{
		return SNew(SBox)
			.Padding(FMargin(4.0f, 0.0f))//文字离边框4像素
			.VAlign(VAlign_Center)				//居中
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::FromInt(_Item->UseNum)))
			];    //数据转化 _Item->UseNum 是个整型（int32），UI 只能显示 FText。所以用了 FString::FromInt() 转成字符串，再用 FText::FromString() 转成 UI 文本。
	}
	else if (ColumnName == TEXT("Index"))  //渲染“index”列
	{
		return SNew(SBox)
			.Padding(FMargin(4.0f, 0.0f))
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::FromInt(_Item->Index)))
			];
	}
	else
	{
		if (ColumnName == TEXT("Path"))  //渲染“Path”列
		{
			return SNew(SBox)
				.Padding(FMargin(4.0f, 0.0f))
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(_Item.IsValid() ? FText::FromString(*_Item->MeshPath) : FText())
				];
		}
		else if (ColumnName == TEXT("Name"))
		{
			return SNew(SBox)
				.Padding(FMargin(4.0f, 0.0f))
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(_Item.IsValid() ? FText::FromString(*_Item->MeshName) : FText())
				];
		}
	}

	return SNullWidget::NullWidget;
}

// 简单参考：SDeviceApps|SDeviceAppsAppListRow
void SMeshUsageItemView::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView,
                                   const TWeakPtr<FMeshUsageItem>& InItem)
{
	ItemData = InItem;
	SMultiColumnTableRow<FMeshUsageItemPtr>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

void SMeshUsageListView::Construct(const FArguments& InArgs)
{
	auto FilterTextBox =
		SNew(SSearchBox)
		.InitialText(FText::FromString(FilterString))
		.HintText(FText::FromString(TEXT("过滤路径...")))
		.OnTextCommitted_Lambda([this](const FText& Text, ETextCommit::Type Type)
		{
			if (bFindFired == false && Text.IsEmpty()) // 防止第一次为空时候触发搜索
				return;

			bFindFired = true;
			FilterString = Text.ToString();

			QueryAndFilterMeshes();   //(在这里：TActorIterator 遍历世界 ─> 统计 Mesh ─> 填充 SceneDataListFiltered 数组)

			// 数据更新，UI不会自动更新
			if (ListView.IsValid())
				ListView->RequestListRefresh();    //(通知 Slate 底层：“我的数据变了，你赶紧重新去画界面！”)

			// if (SceneDataListFiltered.IsEmpty())
			// {
			// 	JLog::ErrorPopup(TEXT("查找过滤结果为空..."));
			// }
		});
	
	
	//OnTextCommitted_Lambda  在 Construct 执行时：程序运行到这里，它的唯一工作是：“记住这行承诺”。它告诉引擎：“如果未来有人用鼠标点击了这个按钮，请执行大括号 {} 里面的代码。” 记完之后，程序就直接跳过大括号，继续往下走了。
	//在 Construct 结束很久以后：Construct 早已执行完毕。此时用户坐在电脑前，用鼠标点击了屏幕上的“查找”按钮。这时，虚幻引擎的鼠标点击事件被触发，引擎才会回过头来，单独执行大括号里面的：
	

	if (false)
	auto FilterTextBox2 = // SNew(SEditableText)
			SNew(SEditableTextBox) // 可以设置背景色
			.Text(FText::FromString(FilterString))
			.HintText(FText::FromString(TEXT("过滤路径...")))
			.OnTextCommitted_Lambda([this](const FText& Text, ETextCommit::Type Type)
			{
				FilterString = Text.ToString();

				QueryAndFilterMeshes();

				// 数据更新，UI不会自动更新
				if (ListView.IsValid())
					ListView->RequestListRefresh();

				// if (SceneDataListFiltered.IsEmpty())
				// {
				// 	JLog::ErrorPopup(TEXT("查找过滤结果为空..."));
				// }
			});

	auto Layout = SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text_Lambda([this]()
				{
					return FText::FromString(FString::Printf(TEXT("当前场景 : %s\t%d个Actor\t%d个Mesh\t%d个过滤Mesh"),
					                                         *LevelName, NumActor, NumMesh, NumMeshFiltered));
				})
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.MinDesiredWidth(200)
				[
					FilterTextBox
				]
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("查找")))
				.OnClicked_Lambda([this]()    //OnTextCommitted_Lambda：Construct 时只注册回调；用户后来在搜索框按 Enter 时才执行里面的代码。
				{
					bFindFired = true;
					QueryAndFilterMeshes();

					// 数据更新，UI不会自动更新
					if (ListView.IsValid())
						ListView->RequestListRefresh();

					// if (SceneDataListFiltered.IsEmpty())
					// {
					// 	JLog::ErrorPopup(TEXT("查找结果为空..."));
					// }

					return FReply::Handled();
				})
			]
		]

		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(ListView, SListView<FMeshUsageItemPtr>)
			.ListItemsSource(&SceneDataListFiltered)
			.SelectionMode(ESelectionMode::Single)
			.OnGenerateRow(this, &SMeshUsageListView::OnGenerateRow)
			.OnSelectionChanged(this, &SMeshUsageListView::OnSelectionChanged)
			.OnMouseButtonDoubleClick(this, &SMeshUsageListView::OnItemDoubleClicked)
			.OnContextMenuOpening(this, &SMeshUsageListView::OnContextMenu)
			.HeaderRow
			(
				SNew(SHeaderRow)

				+ SHeaderRow::Column("Index")
				.FillWidth(0.1f)
				.DefaultLabel(NSLOCTEXT("MeshUsageColumnHeader", "Index", "索引"))

				+ SHeaderRow::Column("Name")
				.FillWidth(0.2f)
				.DefaultLabel(NSLOCTEXT("MeshUsageColumnHeader", "Desc", "模型名字"))

				+ SHeaderRow::Column("Path")
				.FillWidth(0.5f)
				.DefaultLabel(NSLOCTEXT("MeshUsageColumnHeader", "Path", "模型路径"))

				+ SHeaderRow::Column("Num")
				.FillWidth(0.1f)
				.DefaultLabel(NSLOCTEXT("MeshUsageColumnHeader", "Num", "使用数量"))

			)
		];

	ChildSlot
	[
		Layout
	];
}

TSharedRef<ITableRow> SMeshUsageListView::OnGenerateRow(FMeshUsageItemPtr Item,
                                                        const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SMeshUsageItemView, OwnerTable, Item);
}

void SMeshUsageListView::OnSelectionChanged(FMeshUsageItemPtr Selection, ESelectInfo::Type SelectInfo)
{
	if (SelectInfo == ESelectInfo::Type::OnMouseClick)
		CurrentSelection = Selection;
}

//用户鼠标右键弹出的列表 （如下面的 浏览资产   选中关联组件  拷贝路径等功能）
TSharedPtr<SWidget> SMeshUsageListView::OnContextMenu() const
{
	FMenuBuilder MenuBuilder(/*bInShouldCloseWindowAfterMenuSelection=*/true, nullptr, nullptr, /*bCloseSelfOnly=*/
	                                                                    true);

	MenuBuilder.BeginSection("MeshUsageView");
	{
		MenuBuilder.AddMenuEntry(
			NSLOCTEXT("MeshUsageItemView", "SyncLabel", "浏览资产"),
			FText(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([this]()
			{
				if (CurrentSelection.IsValid())
				{
					// 同步选中
					UEditorAssetLibrary::SyncBrowserToObjects({CurrentSelection->MeshPath});
				}
			}))
		);

		MenuBuilder.AddMenuEntry(
			NSLOCTEXT("MeshUsageItemView", "FindActorLabel", "选中关联组件"),
			FText(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([this]()
			{
				SelectSceneComponents();
			}))
		);

		// 拷贝到剪切板
		MenuBuilder.AddMenuEntry(
			NSLOCTEXT("MeshUsageItemView", "ClipboardCopyPath", "拷贝路径"),
			FText(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([this]()
			{
				if (CurrentSelection.IsValid())
				{
					const auto& Text = CurrentSelection->MeshPath;
					if (!Text.IsEmpty())
						FPlatformApplicationMisc::ClipboardCopy(*Text);
				}
			}))
		);

		MenuBuilder.AddMenuEntry(
			NSLOCTEXT("MeshUsageItemView", "ClipboardCopyName", "拷贝名字"),
			FText(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([this]()
			{
				if (CurrentSelection.IsValid())
				{
					const auto& Text = CurrentSelection->MeshName;
					if (!Text.IsEmpty())
						FPlatformApplicationMisc::ClipboardCopy(*Text);
				}
			}))
		);
	}

	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void SMeshUsageListView::OnItemDoubleClicked(FMeshUsageItemPtr Item) const
{
	if (Item.IsValid())
	{
		// 同步选中
		UEditorAssetLibrary::SyncBrowserToObjects({Item->MeshPath});
	}
}

static const FName g_MeshUsageListViewTabId = TEXT("Tool_MeshUsageListView");

void SMeshUsageListView::Register()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(g_MeshUsageListViewTabId,
	                                                  FOnSpawnTab::CreateLambda([](const FSpawnTabArgs& Args)
	                                                  {
		                                                  return SNew(SDockTab)
			                                                  [
				                                                  SNew(SMeshUsageListView)
			                                                  ];
	                                                  }))
	                        .SetDisplayName(NSLOCTEXT("MeshUsageListView", "MeshUsageListViewTitle", "场景Mesh使用统计"))
	                        .SetMenuType(ETabSpawnerMenuType::Hidden);
}

// TODO : 可拓展过滤方式|过滤路径、名字
void SMeshUsageListView::QueryAndFilterMeshes()
{
	NumActor = 0;
	NumMesh = 0;
	NumMeshFiltered = 0;

	auto Level = FJSceneTool::GetCurrentPersistentLevel();
	if (!Level)
	{
		LevelName = TEXT("Untitled");
		return;
	}

	UWorld* World = Level->GetWorld();

	// LevelName = Level->GetPathName();
	LevelName = Level->GetOutermost()->GetPathName();

	// 清除
	SceneDataListSource.Empty();
	SceneDataListFiltered.Empty();  //先把这个数组为空


	TMap<UStaticMesh*, FMeshUsageItemPtr> DataPool;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		TInlineComponentArray<UActorComponent*> Components;
		Actor->GetComponents(Components);
		++NumActor;

		for (UActorComponent* Component : Components)
		{
			if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Component))
			{
				if (auto Mesh = MeshComp->GetStaticMesh())
				{
					FMeshUsageItemPtr Ptr = nullptr;
					if (!DataPool.Contains(Mesh))
					{
						Ptr = MakeShared<FMeshUsageItem>();
						Ptr->MeshPath = Mesh->GetOutermost()->GetName();
						Ptr->MeshName = FPaths::GetBaseFilename(Ptr->MeshPath);

						DataPool.Emplace(Mesh, Ptr);
						SceneDataListSource.Add(Ptr);

						++NumMesh;
					}
					else
						Ptr = DataPool[Mesh];

					check(Ptr);

					++Ptr->UseNum;
				}
			}
		}
	}

	// 数据排序
	SceneDataListSource.Sort([](const FMeshUsageItemPtr& A, const FMeshUsageItemPtr& B)
	{
		return A->UseNum > B->UseNum;
	});

	// 过滤|添加 Index
	const bool bFilter = !FilterString.IsEmpty();

	for (auto& Data : SceneDataListSource)
	{
		bool bAdd = true;
		if (bFilter && !Data->MeshPath.StartsWith(FilterString))
			bAdd = false;

		if (bAdd)
		{
			Data->Index = (++NumMeshFiltered);
			SceneDataListFiltered.Add(Data);
		}
	}
}

// 选中所有相关的Actor|Components
void SMeshUsageListView::SelectSceneComponents() const
{
	if (!CurrentSelection.IsValid())
		return;

	auto& Path = CurrentSelection->MeshPath;
	UStaticMesh* CurrentMesh = LoadObject<UStaticMesh>(nullptr, *Path);

	if (!CurrentMesh)
		return;

	auto Level = FJSceneTool::GetCurrentPersistentLevel();
	if (!Level)
		return;

	UWorld* World = Level->GetWorld();
	// TSet<AActor*> RelateActors;
	TSet<UPrimitiveComponent*> RelatePrimComponents;
	TSet<UActorComponent*> RelateComponents;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		TInlineComponentArray<UActorComponent*> Components;
		Actor->GetComponents(Components);

		for (UActorComponent* Component : Components)
		{
			if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Component))
			{
				auto Mesh = MeshComp->GetStaticMesh();
				if (Mesh && CurrentMesh == Mesh)
				{
					// RelateActors.Add(Actor);
					RelatePrimComponents.Add(MeshComp);
					RelateComponents.Add(MeshComp);
				}
			}
		}
	}

	if (RelateComponents.IsEmpty())
	{
		JLog::ErrorPopup(TEXT("没有找到对象，可能已经被删除或者切换了新场景\n建议重新点击查找，刷新列表..."));
		return;
	}

	// auto _RelateActors = RelateActors.Array();
	auto _RelateComponents = RelateComponents.Array();
	auto _RelatePrimComponents = RelatePrimComponents.Array();
	// FJSceneTool::FocusActors(_RelateActors);
	// FJSceneTool::RefreshSelection(_RelateActors); // 选中Actor

	FJSceneTool::FocusActors({}, _RelatePrimComponents);
	FJSceneTool::RefreshSelection(_RelateComponents); // 选中组件
}

static FAutoConsoleCommand g_CmdMeshUsageListView(
	TEXT("jasset.tool.scene_mesh_usage"),
	TEXT("查找场景中Mesh使用情况"),
	FConsoleCommandDelegate::CreateLambda(
		[]()
		{
			FGlobalTabmanager::Get()->TryInvokeTab(FTabId(g_MeshUsageListViewTabId));
		})
);

// ==================================================================================
// ==================================================================================
