/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include <glcomp/glcomplabel.h>
#include <glcomp/glcompfont.h>
#include <glcomp/glcompset.h>
#include <glcomp/glutils.h>
#include <common/memory.h>

glCompLabel *glCompLabelNew(glCompObj * par, GLfloat x, GLfloat y,
			    char *text)
{
    glCompLabel *p;
//      glCompCommon* parent=&par->common;
    p = NEW(glCompLabel);
    glCompInitCommon((glCompObj *) p, par, x, y);
    p->objType = glLabelObj;
    p->transparent=1;
    //typedef enum {glPanelObj,glbuttonObj,glLabelObj,glImageObj}glObjType;

    p->text = strdup(text);
    p->common.font = glNewFontFromParent ((glCompObj*)p, text);
    p->common.functions.draw = (glcompdrawfunc_t)glCompLabelDraw;

    return p;
}


int glCompLabelDraw(glCompLabel * p)
{
    glCompCommon ref;
    ref = p->common;
    glCompCalcWidget((glCompCommon *) p->common.parent, &p->common, &ref);
    /*draw background */
    if(!p->transparent)
    {
	glCompSetColor(&p->common.color);
	glBegin(GL_QUADS);
	glVertex3d(ref.refPos.x, ref.refPos.y, ref.refPos.z);
	glVertex3d(ref.refPos.x + ref.width, ref.refPos.y, ref.refPos.z);
	glVertex3d(ref.refPos.x + ref.width, ref.refPos.y + ref.height,
	           ref.refPos.z);
	glVertex3d(ref.refPos.x, ref.refPos.y + ref.height, ref.refPos.z);
	glEnd();
    }
    glCompRenderText(p->common.font, (glCompObj *) p);
    return 1;

}
static void update_font(glCompLabel * p,char* text,char* desc,int fs)
{

    glCompFont* temp=p->common.font;
    if (strlen(text) >512)
	return ;

    p->common.font=glNewFont (p->common.compset,text,&p->common.color,temp->type,desc,fs,temp->is2D);
    if(temp)
	glDeleteFont(temp);
    free(p->text);
    p->text = strdup(text);


}

void glCompLabelSetFontName(glCompLabel * p, char* fontName)
{
    glCompFont* temp=p->common.font;
    update_font(p,p->text,fontName,temp->size);
}
