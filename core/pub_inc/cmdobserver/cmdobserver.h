//! \file cmdobserver.h
//! \brief 定义命令扩展观察者接口 CmdObserver
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMDOBSERVER_H_
#define TOUCHVG_CMDOBSERVER_H_

class MgShape;
class MgMotion;
class GiGraphics;
struct MgCommand;
struct MgCmdManager;

//! 命令扩展观察者接口
/*! \ingroup CORE_COMMAND
    \see CmdObserverDefault
 */
struct CmdObserver {
    //! 图形文档内容加载后的通知
    virtual void onDocLoaded(const MgMotion* sender) = 0;

    //! 进入选择命令时的通知
    virtual void onEnterSelectCommand(const MgMotion* sender) = 0;

    //! 视图销毁前、所有命令卸载后的通知
    virtual void onUnloadCommands(MgCmdManager* sender) = 0;

    //! 选择命令中的上下文操作是否隐藏的回调通知
    virtual bool selectActionsNeedHided(const MgMotion* sender) = 0;
#ifndef SWIG
    //! 对选中的图形增加上下文操作的回调通知
    virtual void addShapeActions(const MgMotion* sender,
        int* actions, int &n, const MgShape* shape) = 0;
#endif
    //! 执行非内置上下文操作的通知
    virtual bool doAction(const MgMotion* sender, int action) = 0;

    //! 在非绘图的命令中执行额外的上下文操作的通知
    virtual bool doEndAction(const MgMotion* sender, int action) = 0;

    //! 在绘图命令中显示额外内容的通知
    virtual void drawInShapeCommand(const MgMotion* sender, 
        MgCommand* cmd, GiGraphics* gs) = 0;

    //! 在选择命令中显示额外内容的通知
    virtual void drawInSelectCommand(const MgMotion* sender, 
        const MgShape* shape, int handleIndex, GiGraphics* gs) = 0;
#ifndef SWIG
    //! 在选择命令中拖放图形后的通知
    virtual void onSelectTouchEnded(const MgMotion* sender, int shapeid,
        int handleIndex, int snapid, int snapHandle,
        int count, const int* ids) = 0;
#endif

    virtual bool onShapeWillAdded(const MgMotion* sender, MgShape* shape) = 0;    //!< 通知将添加图形
    virtual void onShapeAdded(const MgMotion* sender, MgShape* shape) = 0;        //!< 通知已添加图形
    virtual bool onShapeWillDeleted(const MgMotion* sender, MgShape* shape) = 0;  //!< 通知将删除图形
    virtual void onShapeDeleted(const MgMotion* sender, MgShape* shape) = 0;      //!< 通知已删除图形
    virtual bool onShapeCanRotated(const MgMotion* sender, MgShape* shape) = 0;   //!< 通知是否能旋转图形
    virtual bool onShapeCanTransform(const MgMotion* sender, MgShape* shape) = 0; //!< 通知是否能对图形变形
    virtual bool onShapeCanUnlock(const MgMotion* sender, MgShape* shape) = 0;    //!< 通知是否能对图形解锁
    virtual bool onShapeCanUngroup(const MgMotion* sender, MgShape* shape) = 0;   //!< 通知是否能对成组图形解散
    virtual void onShapeMoved(const MgMotion* sender, MgShape* shape, int segment) = 0;   //!< 通知图形已拖动
};

class CmdObserverDefault : public CmdObserver
{
public:
    CmdObserverDefault() {}
    virtual ~CmdObserverDefault() {}

    virtual void onDocLoaded(const MgMotion*) {}
    virtual void onEnterSelectCommand(const MgMotion*) {}
    virtual void onUnloadCommands(MgCmdManager*) {}
    virtual bool selectActionsNeedHided(const MgMotion*) { return false; }
    virtual bool doAction(const MgMotion*, int) { return false; }
    virtual bool doEndAction(const MgMotion*, int) { return false; }
    virtual void drawInShapeCommand(const MgMotion*, MgCommand*, GiGraphics*) {}
    virtual void drawInSelectCommand(const MgMotion*, const MgShape*, int, GiGraphics*) {}

    virtual bool onShapeWillAdded(const MgMotion*, MgShape*) { return true; }
    virtual void onShapeAdded(const MgMotion*, MgShape*) {}
    virtual bool onShapeWillDeleted(const MgMotion*, MgShape*) { return true; }
    virtual void onShapeDeleted(const MgMotion*, MgShape*) {}
    virtual bool onShapeCanRotated(const MgMotion*, MgShape*) { return true; }
    virtual bool onShapeCanTransform(const MgMotion*, MgShape*) { return true; }
    virtual bool onShapeCanUnlock(const MgMotion*, MgShape*) { return true; }
    virtual bool onShapeCanUngroup(const MgMotion*, MgShape*) { return true; }
    virtual void onShapeMoved(const MgMotion*, MgShape*, int) {}
#ifndef SWIG
    virtual void addShapeActions(const MgMotion*,int*, int &, const MgShape*) {}
    virtual void onSelectTouchEnded(const MgMotion*,int,int,int,int,int,const int*) {}
#endif
};

#endif // TOUCHVG_CMDOBSERVER_H_
