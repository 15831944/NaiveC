//
//  NSObject+NCInvocation.h
//  NaiveC
//
//  Created by Liang,Zhiyuan(GIS) on 2018/2/15.
//  Copyright © 2018年 Ogreaxe. All rights reserved.
//

#import <Foundation/Foundation.h>

#include <stdio.h>
#include "NCClassInstance.hpp"

@interface NSObject (NCInvocation)

//+(BOOL)invoke:(NSString*)methodName target:(id)target arguments:(vector<shared_ptr<NCStackElement>> &)arguments stack:(vector<shared_ptr<NCStackElement>>& )lastStack;
+(BOOL)invoke:(NSString*)methodName object:(NSObject*)aObject orClass:(Class)aClass arguments:(vector<shared_ptr<NCStackElement>> &)arguments stack:(vector<shared_ptr<NCStackElement>>& )lastStack;

-(BOOL)invoke:(NSString*)methodName arguments:(vector<shared_ptr<NCStackElement>> &)arguments stack:(vector<shared_ptr<NCStackElement>>& )lastStack;

-(shared_ptr<NCStackElement>)attributeForName:(const string & )attrName;

@end
