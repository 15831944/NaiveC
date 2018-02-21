//
//  NSObject+NCInvocation.m
//  NaiveC
//
//  Created by Liang,Zhiyuan(GIS) on 2018/2/15.
//  Copyright © 2018年 Ogreaxe. All rights reserved.
//

#import "NSObject+NCInvocation.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <objc/runtime.h>
#import <objc/message.h>

#include "NCCocoaBox.hpp"
#include "NCCocoaToolkit.hpp"

#include "NCAST.hpp"

@implementation NSObject (NCInvocation)

+(BOOL)invoke:(NSString*)methodName object:(NSObject*)aObject orClass:(Class)aClass arguments:(vector<shared_ptr<NCStackElement>> &)arguments stack:(vector<shared_ptr<NCStackElement>>& )lastStack{
    unsigned int methodCount = 0;
    
    Method * methodList = NULL;
    
    Class targetClass = NULL;
    
    if (aObject) {
        targetClass = aObject.class;
    }
    else if(aClass){
        targetClass = object_getClass(aClass);
    }
    else {
        return NO;
    }
    
    BOOL res = NO;
    while (targetClass) {
        methodList = class_copyMethodList(targetClass, &methodCount);
        for (int i=0; i<methodCount; i++) {
            Method aMethod = methodList[i];
            
            NSString * selectorString = NSStringFromSelector(method_getName(aMethod));
            
            NSString * convertedString = [self convertSelectorString:selectorString];
            
            if([methodName isEqualToString:convertedString]){
                res = [NSObject private_invoke:aMethod target:aObject?aObject:aClass arguments:arguments stack:lastStack];
                free(methodList);
                return res;
            }
        }
        
        free(methodList);
        targetClass = targetClass.superclass;
    }
    
    return res;
}

+(BOOL)private_invoke:(Method)method target:(id)target arguments:(vector<shared_ptr<NCStackElement>> &)arguments stack:(vector<shared_ptr<NCStackElement>>& )lastStack{
    int argCount = method_getNumberOfArguments(method);
    
    if(argCount != arguments.size() + 2){
        NSLog(@"argument count not matched");
        return NO;
    }
    
    SEL selector = method_getName(method);
    NSMethodSignature * signature = [target methodSignatureForSelector:selector];
    
    NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:signature];
    [invocation setSelector:selector];
    [invocation setTarget:target];
    
    for(int i=0;i<argCount-2;i++){
        char argumentType[16];
        method_getArgumentType(method, i, argumentType, 16);
        
        int argPos = i+2;
        
//#define COMP_ENCODE(type, type2) (type[0] == (@encode(type2))[0] && type[1] == (@encode(type2))[1])
#define COMP_ENCODE(type, type2) (strcmp(type,@encode(type2)) == 0)
        
        if(COMP_ENCODE(argumentType, int) ||
           COMP_ENCODE(argumentType, unsigned int )||
           COMP_ENCODE(argumentType, long) ||
           COMP_ENCODE(argumentType, unsigned long )||
           COMP_ENCODE(argumentType, long long ) ||
           COMP_ENCODE(argumentType, unsigned long long )){
            NSNumber * num = [NSNumber numberWithInt:arguments[i]->toInt()];
            [invocation setArgument:&num atIndex:argPos];
        }
        else if(COMP_ENCODE(argumentType, float) ||
                COMP_ENCODE(argumentType, double)){
            NSNumber * num = [NSNumber numberWithDouble:arguments[i]->toFloat()];
            [invocation setArgument:&num atIndex:argPos];
        }
//        else if(COMP_ENCODE(argumentType, NSString)){
//            NSString * str = [NSString stringWithUTF8String:arguments[i]->toString().c_str()];
//            [inv setArgument:&str atIndex:argPos];
//        }
//        else if(strcmp(argumentType,@encode(CGRect)) == 0){
        else if(COMP_ENCODE(argumentType, CGRect)){
            if(dynamic_pointer_cast<NCStackPointerElement>(arguments[i])){
                auto pFrameContainer = dynamic_pointer_cast<NCStackPointerElement>(arguments[i]);
                auto pObject = pFrameContainer->getPointedObject();
                if(pObject && dynamic_pointer_cast<NCFrame>(pObject)){
                    auto pFrame = dynamic_pointer_cast<NCFrame>(pObject);
                    CGRect frame = CGRectMake(pFrame->getX(), pFrame->getY(), pFrame->getWidth(), pFrame->getHeight());
                    [invocation setArgument:&frame atIndex:argPos];
                }
            }
        }
//        else if(strcmp(argumentType,@encode(CGSize)) == 0){
        else if(COMP_ENCODE(argumentType, CGSize)){
            if(dynamic_pointer_cast<NCStackPointerElement>(arguments[i])){
                auto pFrameContainer = dynamic_pointer_cast<NCStackPointerElement>(arguments[i]);
                auto pObject = pFrameContainer->getPointedObject();
                if(pObject && dynamic_pointer_cast<NCSize>(pObject)){
                    auto pSize = dynamic_pointer_cast<NCSize>(pObject);
                    CGSize size = CGSizeMake(pSize->getWidth(), pSize->getHeight());
                    [invocation setArgument:&size atIndex:argPos];
                }
            }
        }
//        else if(strcmp(argumentType,@encode(CGPoint)) == 0){
        else if(COMP_ENCODE(argumentType, CGPoint)){
            if(dynamic_pointer_cast<NCStackPointerElement>(arguments[i])){
                auto pFrameContainer = dynamic_pointer_cast<NCStackPointerElement>(arguments[i]);
                auto pObject = pFrameContainer->getPointedObject();
                if(pObject && dynamic_pointer_cast<NCPoint>(pObject)){
                    auto pPoint = dynamic_pointer_cast<NCPoint>(pObject);
                    CGSize point = CGSizeMake(pPoint->getX(), pPoint->getY());
                    [invocation setArgument:&point atIndex:argPos];
                }
            }
        }
        else if(COMP_ENCODE(argumentType, id)){
            auto& stackElement = arguments[i];
            id realObj = NULL;
            if(dynamic_pointer_cast<NCStackStringElement>(stackElement)){
                auto pstr = dynamic_pointer_cast<NCStackStringElement>(stackElement);
                NSString * nsstr = [NSString stringWithUTF8String: pstr->toString().c_str()];
                realObj = nsstr;
            }
            else if(dynamic_pointer_cast<NCStackPointerElement>(stackElement)){
                auto pointerContainer = dynamic_pointer_cast<NCStackPointerElement>(stackElement);
                auto payloadObj = pointerContainer->getPointedObject();
                if(payloadObj && dynamic_pointer_cast<NCCocoaBox>(payloadObj)){
                    auto cocoabox = dynamic_pointer_cast<NCCocoaBox>(payloadObj);
                    id cocoaObj = (id)CFBridgingRelease(cocoabox->getContent());
                    realObj = cocoaObj;
                    
                }
            }
            [invocation setArgument:&realObj atIndex:argPos];
        }
        else {
            id argnil = NULL;
            [invocation setArgument:&argnil atIndex:argPos];
        }
    }
    
    [invocation invoke];
    
    const char * returnType = signature.methodReturnType;
    
//    if(strcmp(returnType,@encode(void)) == 0){
    if(COMP_ENCODE(returnType, void)){
        return YES;
    }
//    else if(strcmp(returnType,@encode(id)) == 0){
    if(COMP_ENCODE(returnType, id)){
        __unsafe_unretained id result;
        
        [invocation getReturnValue:&result];
        
        NCCocoaBox * box = new NCCocoaBox((void*)CFBridgingRetain(result));
        
        NCStackPointerElement * pRet = new NCStackPointerElement(shared_ptr<NCObject>( box));
        
        lastStack.push_back(shared_ptr<NCStackElement>(pRet));
        
        return YES;
    }
    else {
        NSUInteger length = signature.methodReturnLength;
        void * buffer = (void *)malloc(length);
        [invocation getReturnValue:buffer];
        
        if(COMP_ENCODE(returnType, unsigned int )){
            unsigned int *pret = (unsigned int *)buffer;
            lastStack.push_back(shared_ptr<NCStackIntElement>(new NCStackIntElement( (*pret))));
        }
        else if(COMP_ENCODE(returnType, int)){
            
        }
        else if(COMP_ENCODE(returnType, unsigned long )){
            
        }
        else if(COMP_ENCODE(returnType, long)){
            
        }
        else if(COMP_ENCODE(returnType, unsigned long long)){
            
        }
        else if(COMP_ENCODE(returnType, long long)){
            
        }
        else if(COMP_ENCODE(returnType, double)){
            
        }
        else if(COMP_ENCODE(returnType, float)){
            
        }
        else if(COMP_ENCODE(returnType, CGRect)){
            
        }
        else if(COMP_ENCODE(returnType, CGSize)){
            
        }
        else if(COMP_ENCODE(returnType, CGPoint)){
            
        }
    }
    
    return YES;
}
/*
 covert from A:B:C to A_B_C
 */
+(NSString *) convertSelectorString:(NSString *) selectorString{
//    NSArray * comps = [selectorString componentsSeparatedByString:@":"];
//    NSMutableString * str = [NSMutableString string];
//    [comps enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
//        [str appendString:obj];
//        if(idx != comps.count-1){
//            [str appendString:@"_"];
//        }
//    }];
//    return str;
    
    NSString * converted = [selectorString stringByReplacingOccurrencesOfString:@":" withString:@"_"];
    if ([selectorString characterAtIndex:selectorString.length-1] == ':') {
        converted = [converted substringToIndex:converted.length-1];
    }
    return converted;
}

-(BOOL)invoke:(NSString*)methodName arguments:(vector<shared_ptr<NCStackElement>> &)arguments stack:(vector<shared_ptr<NCStackElement>>& )lastStack{
    
    return [NSObject invoke:methodName object:self orClass:nil arguments:arguments stack:lastStack];
}

-(shared_ptr<NCStackElement>)attributeForName:(const string & )attrName{
    return nullptr;
}

@end
