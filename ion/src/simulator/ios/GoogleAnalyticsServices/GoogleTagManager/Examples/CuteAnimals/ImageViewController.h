//
//  ImageViewController.h
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "NavController.h"

@interface ImageViewController : UIViewController

@property(nonatomic, retain) NavController *navController;
@property(nonatomic, retain) UIImageView *imageView;

- (id)initWithNibName:(NSString *)nibNameOrNil
               bundle:(NSBundle *)nibBundleOrNil
                title:(NSString *)title
                image:(UIImage *)image;

@end
