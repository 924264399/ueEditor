#pragma once

struct FMeshUsageItem 
{
	FString MeshPath;
	FString MeshName;
	int32 UseNum = 0; // mesh 相同的 actor 数量
	int32 Index = 0;
}; //这是底层的数据结构（Model），代表列表中的“每一行”所需要的数据。
//MeshPath 和 MeshName：记录模型的资产路径和名称。
//UseNum：记录这个模型在当前场景中被多少个 Actor 使用了。
//用于在 UI 上显示的序号

using FMeshUsageItemPtr = TSharedPtr<FMeshUsageItem>; //智能指针

// SAssetColumnView
// SSourceControlChangelistsWidget
// SListView
// STreeView


///这个是单行 试图
class SMeshUsageItemView : public SMultiColumnTableRow<FMeshUsageItemPtr> // 这是列表中每一行的 UI 控件。它继承自 SMultiColumnTableRow，说明这是一个多列的表格行。
{
public:
	SLATE_BEGIN_ARGS(SMeshUsageItemView)
		{
		}

	SLATE_END_ARGS()   //SLATE_BEGIN_ARGS / SLATE_END_ARGS：Slate 的标准宏，用于定义构建这个 Widget 时可以传入的参数（这里目前为空）。

	
	//// Construct：当 ListView 需要显示某一行数据时，会调用此函数实例化该行对象，完成该行 UI 的基础布局。
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView,
	               const TWeakPtr<FMeshUsageItem>& InItem);
	
	
	
	//这是多列表格最核心的重载函数。它的作用是：根据列的名字（比如 "Index", "Name", "Path"），返回对应单元格里应该显示什么 UI（比如一个文本块 STextBlock）。
	//GenerateWidgetForColumn：装修工。当这一行被确定显示后，ListView 会根据表格列配置，依次调用此函数，填充每一格的具体 UI。
	//这个说白了就是 每个格子都调用一次  然后填入什么数据  非常核心了可以说
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	
	

private:
	TWeakPtr<FMeshUsageItem> ItemData;//ItemData 是一个弱指针（TWeakPtr），指向上面定义的 FMeshUsageItem。使用弱指针可以防止循环引用导致的内存泄漏。
};


//这是主面板视图：SMeshUsageListView
//这是整个工具的主容器，包含了顶部的搜索栏、统计信息以及下方的整个数据列表。它继承自 SCompoundWidget，这是自定义复合 Slate 控件最常用的基类。
class SMeshUsageListView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMeshUsageListView)
		{
		}

	SLATE_END_ARGS()

	// Construct：大楼的总建筑师。执行一次，完成搜索框、统计栏、列表骨架的搭建。
	void Construct(const FArguments& InArgs);
	static void Register(); //Register() 是一个静态方法，通常用于将这个面板注册到 UE的Tab  这个必须放在我们模块cpp的初始化函数里

private:
	TSharedPtr<SWidget> OnContextMenu() const;               //用户鼠标右键弹出的列表 套路函数（可选）  它绑定在 ListView 的 .OnContextMenuOpening(this, &SMeshUsageListView::OnContextMenu) 上。
															//只要你想让用户在列表里“点右键弹出菜单”，在 Slate 里固定的套路就是写一个返回 TSharedPtr<SWidget> 的函数，里面用 FMenuBuilder 来组装菜单。
	TSharedRef<ITableRow> OnGenerateRow(FMeshUsageItemPtr Item, 
	                                    const TSharedRef<STableViewBase>& OwnerTable);//必写核心套路函数  只要你用列表，就必须写一个函数来告诉列表“数据怎么变成这一行的 UI”。
	
	void OnSelectionChanged(FMeshUsageItemPtr Selection, ESelectInfo::Type SelectInfo);//选中交互套路（可选） 它绑定在 .OnSelectionChanged(...) 上。当用户在界面上用鼠标点击选中了某一行，Slate UI 就会自动触发这个函数。
																						//它的套路就是用来记录“当前谁被选中了”（即代码里的 CurrentSelection = Selection;）
	
	
	
	void OnItemDoubleClicked(FMeshUsageItemPtr Item) const; //业务函数 双击同步资产 它的内部实现——它是执行了 UEditorAssetLibrary::SyncBrowserToObjects。双击这一行，要在内容浏览器里高亮定位这个 Mesh 资产。

	void QueryAndFilterMeshes();  //业务函数  场景数据扫描与过滤    这是整个工具的核心数据源头。它负责用 TActorIterator 去搜寻整个世界（World）中的 Actor，然后找出 UStaticMeshComponent 进行数量统计，最后还要处理搜索框的字符串过滤
	void SelectSceneComponents() const;//业务函数 反向选中场景物体  你看它在右键菜单“选中关联组件”时被调用。它的功能是利用 FJSceneTool::RefreshSelection 跑到场景里把所有用了这个 Mesh 的 Actor/Component 真的给勾选上
	
	
	//注意上面的FMeshUsageItemPtr Selection是什么意思？ 我们已经知道FMeshUsageItemPtr是共享智能指针（结构体的）
	//当你在函数的参数列表里写 FMeshUsageItemPtr Selection 时，这意味着值传递（Pass by Value）
	//它绝对没有在内存中重新 new 或创建一个新的 FMeshUsageItem 结构体对象。场景里原本有 100 个 Mesh，现在依然只有 100 个，没有变成 101 个。
	//
	

	int32 NumActor = 0;
	int32 NumMesh = 0;
	int32 NumMeshFiltered = 0;
	bool bFindFired = false;
	FString LevelName = TEXT("Untitled");
	FString FilterString;

	TArray<FMeshUsageItemPtr> SceneDataListSource;
	TArray<FMeshUsageItemPtr> SceneDataListFiltered;

	TSharedPtr<SListView<FMeshUsageItemPtr>> ListView;
	FMeshUsageItemPtr CurrentSelection;
};
