//
//  ImageViewController.m
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

#import "ImageViewController.h"

#import "AppDelegate.h"
#import "TAGDataLayer.h"
#import "TAGManager.h"
#import "Utils.h"

@interface ImageViewController ()

@property(nonatomic, assign) AppDelegate *delegate;

@end

@implementation ImageViewController

@synthesize imageView = _imageView;
@synthesize navController = _navController;

- (id)initWithNibName:(NSString *)nibNameOrNil
               bundle:(NSBundle *)nibBundleOrNil
                title:(NSString *)title
                image:(UIImage *)image {
  self = [super initWithNibName:nibNameOrNil
                         bundle:nibBundleOrNil];
  if (self) {
    self.title = title;
    self.imageView = [[UIImageView alloc] initWithImage:image];
    self.imageView.userInteractionEnabled = YES;
    self.imageView.contentMode = UIViewContentModeScaleAspectFit;
  }
  return self;
}

// Override
- (void)viewDidLoad {
  [super viewDidLoad];
  // Put the image_name into the data layer for future use.
  [self.delegate.tagManager.dataLayer pushValue:self.title
                                         forKey:@"category_name"];
}

// Override
- (void)viewWillAppear:(BOOL)animated {
  [super viewWillAppear:animated];
  [Utils pushOpenScreenEventWithScreenName:@"ImageViewScreen"];
}

// Override
- (void)viewWillDisappear:(BOOL)animated {
  [super viewDidDisappear:animated];
  [Utils pushCloseScreenEventWithScreenName:@"ImageViewScreen"];
}

// Override
- (BOOL)shouldAutorotateToInterfaceOrientation:
    (UIInterfaceOrientation)interfaceOrientation {
  return YES;
}

@end
