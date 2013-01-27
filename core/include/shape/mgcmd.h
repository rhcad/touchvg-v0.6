//! \file mgcmd.h
//! \brief 定义绘图命令接口
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_H_
#define __GEOMETRY_MGCOMMAND_H_

#include <gigraph.h>
#include <mgshapes.h>
#include <mgshape.h>

struct MgSelection;
struct MgView;
struct MgMotion;
struct MgCommand;
class MgBaseCommand;
struct MgCommandManager;
struct MgSnap;
struct MgActionDispatcher;

//! 返回命令管理器.
/*! \ingroup CORE_COMMAND
 */
MgCommandManager* mgGetCommandManager();

//! 注册外部命令
/*! \ingroup CORE_COMMAND
    \param name 命令名称，例如 YourCmd::Name()
    \param factory 命令类的创建函数，例如 YourCmd::Create, 为NULL则取消注册
*/
void mgRegisterCommand(const char* name, MgCommand* (*factory)());

//! 注册图形实体类型
/*! \ingroup GEOM_SHAPE
    \param type MgBaseShape 派生图形类的 Type()，或 MgShapeT(图形类)的 Type()
    \param factory 图形类的创建函数，例如 MgShapeT(图形类)的 create, 为NULL则取消注册
*/
void mgRegisterShapeCreator(int type, MgShape* (*factory)());

//! 图形视图接口
/*! \ingroup CORE_COMMAND
    \interface MgView
*/
struct MgView {
    virtual MgShapes* shapes() = 0;             //!< 得到图形列表
    virtual GiTransform* xform() = 0;           //!< 得到坐标系对象
    virtual GiGraphics* graph() = 0;            //!< 得到图形显示对象
    virtual void regen() = 0;                   //!< 标记视图待重新构建显示
    virtual void redraw(bool fast) = 0;         //!< 标记视图待更新显示
    
    virtual GiContext* context() {              //!< 得到当前绘图属性
        return shapes() ? shapes()->context() : NULL; }
    virtual bool useFinger() { return true; }   //!< 使用手指或鼠标交互
    virtual void selChanged() {}                //!< 选择集改变的通知
    
    virtual bool shapeWillAdded(MgShape* shape) {       //!< 通知将添加图形
        return !!shape; }
    virtual void shapeAdded(MgShape* shape) {           //!< 通知已添加图形，由视图重新构建显示
        if (shape) regen(); }
    virtual bool shapeWillDeleted(MgShape* shape) {     //!< 通知将删除图形
        return !!shape; }
    virtual bool removeShape(MgShape* shape) {          //!< 删除图形
        return !!shape->getParent()->removeShape(shape->getID()); }
    virtual bool shapeCanRotated(MgShape* shape) {      //!< 通知是否能旋转图形
        return !!shape; }
    virtual bool shapeCanTransform(MgShape* shape) {    //!< 通知是否能对图形变形
        return !!shape; }
    virtual void shapeMoved(MgShape* shape, int segment) {  //!< 通知图形已拖动
        if (shape && segment) segment=1; }
    
    virtual bool isContextActionsVisible() { return false; }
    virtual bool showContextActions(int selState, const int* actions,
                                    const Box2d& selbox, const MgShape* shape) { //!< 显示上下文菜单
        return selState < 0 && actions && selbox.isEmpty() && shape; }
    virtual bool drawHandle(GiGraphics* gs, const Point2d& pnt, bool hotdot) {  //!< 显示控制点
        return gs && pnt != pnt && hotdot; }
};

//! 命令参数
/*! \ingroup CORE_COMMAND
*/
struct MgMotion {
    MgView*     view;                           //!< 图形视图
    float       velocity;                       //!< 移动速度，像素每秒
    bool        dragging;                       //!< 是否正按下拖动
    bool        pressDrag;                      //!< 是否为一指按住并另一手指拖动
    Point2d     startPoint;                     //!< 开始点，视图坐标
    Point2d     startPointM;                    //!< 开始点，模型坐标
    Point2d     lastPoint;                      //!< 上次点，视图坐标
    Point2d     lastPointM;                     //!< 上次点，模型坐标
    Point2d     point;                          //!< 当前点，视图坐标
    Point2d     pointM;                         //!< 当前点，模型坐标
    MgMotion() : view(NULL), velocity(0), dragging(false), pressDrag(false) {}
};

//! 命令接口
/*! \ingroup CORE_COMMAND
    \interface MgCommand
    \see mgGetCommandManager, mgRegisterCommand, MgBaseCommand
*/
struct MgCommand {
    virtual const char* getName() const = 0;                //!< 返回命令名称
    virtual void release() = 0;                             //!< 销毁对象
    
    virtual bool cancel(const MgMotion* sender) = 0;        //!< 取消命令
    virtual bool initialize(const MgMotion* sender) = 0;    //!< 开始命令
    virtual bool undo(bool &enableRecall, const MgMotion* sender) = 0;  //!< 回退一步
    
    virtual bool draw(const MgMotion* sender, GiGraphics* gs) = 0;  //!< 显示动态图形
    virtual void gatherShapes(const MgMotion* sender, MgShapes* shapes) = 0;   //!< 得到动态图形
    virtual MgShape* getCurrentShape(const MgMotion*) { return NULL; } //!< 获得当前图形
    
    virtual bool click(const MgMotion* sender) = 0;         //!< 点击
    virtual bool doubleClick(const MgMotion* sender) = 0;   //!< 双击
    virtual bool longPress(const MgMotion* sender) = 0;     //!< 长按
    virtual bool touchBegan(const MgMotion* sender) = 0;    //!< 开始滑动
    virtual bool touchMoved(const MgMotion* sender) = 0;    //!< 正在滑动
    virtual bool touchEnded(const MgMotion* sender) = 0;    //!< 滑动结束
    virtual bool mouseHover(const MgMotion*) { return false; } //!< 鼠标掠过
    
    virtual bool isDrawingCommand() { return false; }       //!< 是否为绘图命令
    virtual bool isFloatingCommand() { return false; }      //!< 是否可嵌套在其他命令中
    virtual bool doContextAction(const MgMotion*, int) { return false; } //!< 执行上下文动作
    virtual int getDimensions(MgView*, float*, char*, int) { return 0; } //!< 得到各种度量尺寸
};

//! 命令接口的默认实现，可以以此派生新命令类
/*! example: mgRegisterCommand(YourCmd::Name(), YourCmd::Create);
    \ingroup CORE_COMMAND
    \see MgCommandDraw
*/
class MgBaseCommand : public MgCommand {
protected:
    MgBaseCommand() {}
    virtual ~MgBaseCommand() {}
    
    //static const char* Name() { return "yourcmd"; }
    //static MgCommand* Create() { return new YourCmd; }
    
    virtual bool cancel(const MgMotion*) { return false; }
    virtual bool initialize(const MgMotion*) { return true; }
    virtual bool undo(bool &, const MgMotion*) { return false; }
    virtual bool draw(const MgMotion*, GiGraphics*) { return false; }
    virtual void gatherShapes(const MgMotion*, MgShapes*) {}
    virtual bool click(const MgMotion* sender) { return longPress(sender); }
    virtual bool doubleClick(const MgMotion*) { return false; }
    virtual bool longPress(const MgMotion*);
    virtual bool touchBegan(const MgMotion*) { return false; }
    virtual bool touchMoved(const MgMotion*) { return false; }
    virtual bool touchEnded(const MgMotion*) { return false; }
};

//! 命令管理器接口
/*! \ingroup CORE_COMMAND
    \interface MgCommandManager
    \see mgGetCommandManager
*/
struct MgCommandManager {
    virtual const char* getCommandName() = 0;               //!< 得到当前命令名称
    virtual MgCommand* getCommand() = 0;                    //!< 得到当前命令
    virtual bool setCommand(const MgMotion* sender, const char* name) = 0;  //!< 启动命令
    virtual bool cancel(const MgMotion* sender) = 0;        //!< 取消当前命令
    virtual void unloadCommands() = 0;                      //!< 退出时卸载命令
    
    //! 得到当前选择的图形
    /*!
        \param view 当前操作的视图
        \param count 最多获取多少个图形，为0时返回实际个数
        \param shapes 填充当前选择的图形对象
        \param forChange 是否用于修改，用于修改时将复制临时图形，动态修改完后要调用 dynamicChangeEnded()
        \return 获取多少个图形，或实际个数
    */
    virtual int getSelection(MgView* view, int count, MgShape** shapes, bool forChange = false) = 0;
    
    //! 结束动态修改，提交或放弃所改的临时图形
    virtual bool dynamicChangeEnded(MgView* view, bool apply) = 0;
    
    //! 返回选择集对象
    virtual MgSelection* getSelection(MgView* view) = 0;
    
    //! 返回上下文动作分发对象
    virtual MgActionDispatcher* getActionDispatcher() = 0;
    
    //! 执行默认的上下文动作
    virtual void doContextAction(const MgMotion* sender, int action) = 0;
    
    //! 返回图形特征点捕捉器
    virtual MgSnap* getSnap() = 0;
};

#endif // __GEOMETRY_MGCOMMAND_H_
