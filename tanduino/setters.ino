void setPCD8544(float val1, float val2) {
  _PCD8544.clearDisplay();
  _PCD8544.setTextSize(2);
  _PCD8544.setTextColor(BLACK);
  _PCD8544.setCursor(0,0);
  _PCD8544.print(val1);
  _PCD8544.println(F(" deg."));
  _PCD8544.print(val2);
  _PCD8544.println(F(" deg. N"));
  _PCD8544.display();
}
