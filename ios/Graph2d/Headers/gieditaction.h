//! \file GiEditAction.h
//! \brief 定义图形编辑按钮响应协议 GiEditAction
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

//! 图形编辑按钮响应协议
/*! 在 GiViewController 派生类中支持此协议，实现其中的某些函数来定制回调功能。
    \ingroup GRAPH_IOS
 */
@protocol GiEditAction
@optional

- (IBAction)menuClickDraw:(id)sender;
- (IBAction)menuClickSelAll:(id)sender;
- (IBAction)menuClickReset:(id)sender;
- (IBAction)menuClickDelete:(id)sender;
- (IBAction)menuClickClone:(id)sender;
- (IBAction)menuClickClosed:(id)sender;
- (IBAction)menuClickAddNode:(id)sender;
- (IBAction)menuClickDelNode:(id)sender;
- (IBAction)menuClickFixedLength:(id)sender;

- (void)shapeSelChanged;                    //!< 选择集改变的通知
- (BOOL)shapeWillAdded;                     //!< 通知将添加图形
- (void)shapeAdded;                         //!< 通知已添加图形
- (BOOL)shapeWillDeleted;                   //!< 通知将删除图形
- (BOOL)shapeCanRotated;                    //!< 通知是否能旋转图形
- (BOOL)shapeCanTransform;                  //!< 通知是否能对图形变形
- (void)shapeMoved;                         //!< 通知图形已拖动
- (BOOL)longPressSelection;                 //!< 选择状态下长按, MgSelState

@end
