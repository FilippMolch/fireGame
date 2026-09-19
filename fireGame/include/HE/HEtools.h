//
// Created by FIlipp on 12.06.2024.
//

#ifndef HETOOLS_H
#define HETOOLS_H

#include <HE/engineClass.h>
#include <HE/HEmain.h>

typedef struct HEobjectDraw HEobjectDraw;

void fillObj(std::unique_ptr<HEobject>& obj, std::unique_ptr<HEobjectDraw>& objDr);
void fillObj_withoutBuffs(std::unique_ptr<HEobject>& obj, std::unique_ptr<HEobjectDraw>& objDr);

void genGLTextureFotText(GLuint* tex, uint32_t w, uint32_t h, uint8_t* buffer);

#endif //HETOOLS_H
