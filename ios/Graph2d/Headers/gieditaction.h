//! \file GiEditAction.h
//! \brief 定义图形编辑按钮响应协议 GiEditAction
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

@interface GiActionParams : NSObject            //!< showContextActions 参数
@property (nonatomic)   int         selstate;   //!< 选择状态, MgSelState
@property (nonatomic)   const int*  actions;    //!< 上下文动作数组，以0结束，每一项为 MgContextAction 值
@property (nonatomic)   CGRect      selbox;     //!< 选中框或点击位置，为sender视图内的点坐标单位
@property (nonatomic,assign)    UIView*     view;           //!< 触发消息的图形视图
@property (nonatomic,assign)    NSMutableArray* buttons;    //!< 填充将要创建的按钮对象，以便自动销毁
@end

//! 图形编辑按钮响应协议
/*! 在 GiViewController 派生类中支持此协议，实现其中的某些函数来定制回调功能。
    \ingroup GRAPH_IOS
 */
@protocol GiEditAction
@optional

- (void)shapeSelChanged;                    //!< 选择集改变的通知
- (BOOL)shapeWillAdded;                     //!< 通知将添加图形
- (void)shapeAdded;                         //!< 通知已添加图形
- (BOOL)shapeWillDeleted;                   //!< 通知将删除图形
- (BOOL)shapeCanRotated;                    //!< 通知是否能旋转图形
- (BOOL)shapeCanTransform;                  //!< 通知是否能对图形变形
- (void)shapeMoved;                         //!< 通知图形已拖动

- (BOOL)showContextActions:(GiActionParams*)params; //!< 显示上下文按钮

@end
