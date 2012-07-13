//! \file GiEditAction.h
//! \brief 定义图形编辑按钮响应协议 GiEditAction
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

//! 图形编辑按钮响应协议
/*! \ingroup GRAPH_IOS
 */
@protocol GiEditAction
- (IBAction)menuClickDraw:(id)sender;
- (IBAction)menuClickSelAll:(id)sender;
- (IBAction)menuClickReset:(id)sender;
- (IBAction)menuClickDelete:(id)sender;
- (IBAction)menuClickClone:(id)sender;
- (IBAction)menuClickClosed:(id)sender;
- (IBAction)menuClickAddNode:(id)sender;
- (IBAction)menuClickDelNode:(id)sender;
@end
