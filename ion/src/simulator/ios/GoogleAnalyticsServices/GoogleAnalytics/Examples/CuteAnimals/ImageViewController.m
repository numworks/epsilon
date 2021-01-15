//
//  ImageViewController.m
//  CuteAnimals
//
//  Copyright 2012 Google, Inc. All rights reserved.
//

#import "ImageViewController.h"

#import "AppDelegate.h"

@interface ImageViewController ()

@property(nonatomic, strong) UIImage *image;

@end

@implementation ImageViewController

- (id)initWithNibName:(NSString *)nibNameOrNil
               bundle:(NSBundle *)nibBundleOrNil
                title:(NSString *)title
                image:(UIImage *)image {
  self = [super initWithNibName:nibNameOrNil
                         bundle:nibBundleOrNil];
  if (self) {
    _image = image;
    self.title = self.screenName = title;
  }
  return self;
}

- (void)viewDidLoad {
  [super viewDidLoad];
  UIImageView *imageView = [[UIImageView alloc] initWithImage:self.image];
  imageView.userInteractionEnabled = YES;
  imageView.contentMode = UIViewContentModeScaleAspectFit;
  [self.view addSubview:imageView];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:
    (UIInterfaceOrientation)interfaceOrientation {
  return YES;
}

@end
