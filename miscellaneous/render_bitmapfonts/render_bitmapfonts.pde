import java.awt.Desktop; 

int curY = 0;
PFont myFont;
PrintWriter logOutput;

void setup() {

  size(1200, 1000);

  boolean smooth = true;
  boolean crisp = false;
  curY = 0;

  int[] unicodeBlocks = {
    0x0021, 0x007E, //Basic Latin, 128, 128, Latin (52 characters), Common (76 characters)
    0x0080, 0x00FF, //Latin-1 Supplement, 128, 128, Latin (64 characters), Common (64 characters)
    0x0100, 0x017F, //Latin Extended-A, 128, 128, Latin
    0x0180, 0x024F, //Latin Extended-B, 208, 208, Latin
    //0x0250, 0x02AF, //IPA Extensions, 96, 96, Latin
    //0x02B0, 0x02FF, //Spacing Modifier Letters, 80, 80, Bopomofo (2 characters), Latin (14 characters), Common (64 characters)
    //0x0030, 0x0039, //Example custom range (numbers 0-9)
    //0x0041, 0x005A, //Example custom range (Upper case A-Z)
    //0x0061, 0x007A, //Example custom range (Lower case a-z)
    //0xF000, 0xF0FF,  //weather font
  };

  int blockCount = unicodeBlocks.length;
  int firstUnicode = 0;
  int lastUnicode  = 0;
  char[]   charset;
  int  index = 0, count = 0;

  for (int i = 0; i < blockCount; i+=2) {
    firstUnicode = unicodeBlocks[i];
    lastUnicode  = unicodeBlocks[i+1];
    if (lastUnicode < firstUnicode) {
      delay(100);
      System.err.println("ERROR: Bad Unicode range secified, last < first!");
      System.err.print("first in range = 0x" + hex(firstUnicode, 4));
      System.err.println(", last in range  = 0x" + hex(lastUnicode, 4));
      while (true);
    }
    count += (lastUnicode - firstUnicode + 1);
  }

  charset = new char[count];
  for (int i = 0; i < blockCount; i+=2) {
    firstUnicode = unicodeBlocks[i];
    lastUnicode  =  unicodeBlocks[i+1];
    for (int code = firstUnicode; code <= lastUnicode; code++) {
      charset[index] = Character.toChars(code)[0];
      index++;
    }
  }

  String str = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~°ÄÅÆÖØÚÜßáäåæéíöøúüýąČěľłńŘřŚŠź";
  char[] charsetbasic = str.toCharArray();
  str = "ABCDEFGHIJKLMNOPQRSTUVWXYZminortzÁÅÄÖØÚČŚŠ0123456789-";
  char[] charsetdaynames = str.toCharArray();
  str = "0123456789.°-";
  char[] charsetnumbers = str.toCharArray();

  createAndSaveFont("twcondensed", ".ttf", charsetdaynames, 20, crisp);

  createAndSaveFont("bahnschrift", ".ttf", charsetbasic, 20, crisp);
  createAndSaveFont("bahnschrift", ".ttf", charsetbasic, 30, crisp);
  createAndSaveFont("bahnschrift", ".ttf", charsetnumbers, 70, crisp);

  createAndSaveFont("calibrib", ".ttf", charsetbasic, 30, crisp);

  createAndSaveFont("calibrib", ".ttf", charset, 16, crisp);
  createAndSaveFont("REFSAN", ".ttf", charset, 12, crisp);
  createAndSaveFont("tahoma", ".ttf", charset, 9, crisp);

  try {
    String path = sketchPath();
    Desktop.getDesktop().open(new File(path+"/FontFiles"));
  }
  catch(IOException e) {
    println("Failed to create the file");
  }
}

void createAndSaveFont(String fontName, String fontType, char charset[], int fontSize, boolean smooth) {

  int displayFontSize = fontSize;
  myFont = createFont(fontName+fontType, displayFontSize, smooth, charset);
  fill(0, 0, 0);
  textFont(myFont);
  int gapx = displayFontSize*10/8;
  int gapy = displayFontSize*10/8;
  int index = 0;
  fill(0);
  textSize(displayFontSize);
  curY += gapy;
  for (int y = curY; y < height-gapy; y += gapy) {
    int x = 10;
    curY = y;
    while (x < width) {

      int unicode = charset[index];
      float cwidth = textWidth((char)unicode) + 2;
      if ( (x + cwidth) > (width - gapx) ) break;

      text(new String(Character.toChars(unicode)), x, y);

      x += cwidth;
      index++;
      if (index >= charset.length) break;
    }
    if (index >= charset.length) break;
  }

  PFont font;
  font = createFont(fontName + fontType, fontSize, smooth, charset);
  println("Created font " + fontName + str(fontSize) + ".vlw");
  try {
    print("Saving to sketch FontFiles folder... ");
    OutputStream output = createOutput("FontFiles/" + fontName + str(fontSize) + ".vlw");
    font.save(output);
    output.close();
    println("OK!");
    delay(100);
  } 
  catch (IOException e) {
    println("Doh! Failed to create the file");
  }
}
