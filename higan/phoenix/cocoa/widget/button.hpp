@interface CocoaButton : NSButton {
@public
  phoenix::Button *button;
}
-(id) initWith :(phoenix::Button&)button;
-(IBAction) activate :(id)sender;
@end

namespace phoenix {

struct pButton : public pWidget {
  Button &button;
  CocoaButton *cocoaButton;

  Size minimumSize();
  void setImage(const image &image, Orientation orientation);
  void setText(const string &text);

  pButton(Button &button) : pWidget(button), button(button) {}
  void constructor();
  void destructor();
};

}
