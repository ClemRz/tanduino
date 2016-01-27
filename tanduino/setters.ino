void setPCD8544(float val1, float val2) {
  _PCD8544.clearDisplay();
/* What is faster? Clear all or compute the area to clear?
 * TODO: get the boundingBox of the previous text: 
int16_t  x1, y1;
uint16_t w, h;
 
_PCD8544.getTextBounds(string, x, y, &y1, &y1, &w, &h);
*/

  //_PCD8544.setTextSize(1);
  //_PCD8544.setTextColor(BLACK);
  _PCD8544.setTextWrap(PCD8544_TEXT_WRAP);
  _PCD8544.setCursor(0,12); //12 is the value for this font TODO: check how to get it dynamically
  _PCD8544.print(val1);
  _PCD8544.println(); //TODO add degree symbol
  _PCD8544.print(val2);
  _PCD8544.println(F("N")); //TODO add degree symbol
  _PCD8544.display();
}
