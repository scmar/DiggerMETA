
void drawWorld() {
  gb.display.clear();
  for (byte y = 0; y < WORLD_H; y++) {
    for (byte x = 0; x < WORLD_W; x++) {
      //coordinates on the screen depending on the camera position
      int x_screen = x * SPRITE_W - camera_x;
      int y_screen = y * SPRITE_H - camera_y;
      byte sprite = world[x][y];
      sprite &= SPRITEMASK;
      if (sprite == DIAMOND && gb.frameCount % 2) {
        sprite = DIAMOND2;
      }
      if (sprite == PLAYER) sprite = PLAYER + dir;
      if (x_screen < -SPRITE_W || x_screen > gb.display.width() || y_screen < 0 || y_screen > gb.display.height()) {
        continue; // don't draw sprites which are out of the screen
      }
      spritesheet.setFrame(sprite);
      gb.display.drawImage(x_screen, y_screen, spritesheet);
    }
  }
  if (timer == 0) dir = 7; timer--;
  gb.display.setColor(gamemode ? INDEX_BLACK : INDEX_WHITE);
  gb.display.fillRect(0, 0, gb.display.width(), SPRITE_H);
  gb.display.cursorX = 0;
  gb.display.cursorY = 0;
  gb.display.setColor(INDEX_ORANGE); //orange
  gb.display.print(F("\04:"));
  gb.display.print(diamonds);
  gb.display.print(F("/"));
  gb.display.print(diams_need);
  gb.display.setColor(INDEX_RED); //rot
  
  gb.display.print(F(" \03:"));
  gb.display.print(lives);
  gb.display.setColor(gamemode ? INDEX_WHITE : INDEX_BLACK);
  
  gb.display.print(F(" L:"));
  gb.display.print(curLevel + 1);
 //  gb.display.print(F(" "));
  // gb.display.print((int)(1000/(millis()-starttime)));
  // starttime=millis();
  
}

