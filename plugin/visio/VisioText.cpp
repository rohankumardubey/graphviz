/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
 *      This software is part of the graphviz package      *
 *                http://www.graphviz.org/                 *
 *                                                         *
 *            Copyright (c) 1994-2004 AT&T Corp.           *
 *                and is licensed under the                *
 *            Common Public License, Version 1.0           *
 *                      by AT&T Corp.                      *
 *                                                         *
 *        Information and Software Systems Research        *
 *              AT&T Research, Florham Park NJ             *
 **********************************************************/

#include "VisioText.h"

#include "gvcjob.h"
#include "gvio.h"

namespace Visio
{
	static const float INCHES_PER_POINT = 1.0 / 72.0;

	Char::Char(double size, unsigned char red, unsigned char green, unsigned char blue):
		_size(size),
		_red(red),
		_green(green),
		_blue(blue)
	{
	}
	
	void Char::Print(GVJ_t* job) const
	{
		gvputs(job, "<Char>\n");
		gvprintf(job, "<Color>#%02X%02X%02X</Color>\n", _red, _green, _blue);
		gvprintf(job, "<Size>%f</Size>\n", _size * job->scale.x * INCHES_PER_POINT);	/* scale font size, VDX uses inches */
		gvputs(job, "</Char>\n");
	}
	
	Para::Para(HorzAlign horzAlign):
		_horzAlign(horzAlign)
	{
	}
	
	void Para::Print(GVJ_t* job) const
	{
		gvputs(job, "<Para>\n");
		gvprintf(job, "<HorzAlign>%d</HorzAlign>\n", _horzAlign);
		gvputs(job, "</Para>\n");
	}
	
	Run::Run(boxf bounds, char* text):
		_bounds(bounds),
		_text(agstrdup(text))	/* copy text */
	{
	}
	
	Run::~Run()
	{
		/* since we copied, we need to free */
		agstrfree(_text);
	}
	
	boxf Run::GetBounds() const
	{
		return _bounds;
	}
	
	void Run::Print(GVJ_t* job, unsigned int index) const
	{
		gvprintf(job, "<pp IX='%d'/><cp IX='%d'/>%s\n", index, index, _text ? _text : "");	/* para mark + char mark + actual text */
	}
	
	Text* Text::CreateText(GVJ_t* job, pointf p, textpara_t* para)
	{
		Para::HorzAlign horzAlign;
		
		/* compute text bounding box and VDX horizontal align */
		boxf bounds;
		bounds.LL.y = p.y + para->yoffset_centerline;
		bounds.UR.y = p.y + para->yoffset_centerline + para->height;
		double width = para->width;
		switch (para->just)
		{
			case 'r':
				horzAlign = Para::horzRight;
				bounds.LL.x = p.x - width;
				bounds.UR.x = p.x;
				break;
			case 'l':
				horzAlign = Para::horzLeft;
				bounds.LL.x = p.x;
				bounds.UR.x = p.x + width;
				break;
			case 'n':
			default:
				horzAlign = Para::horzCenter;
				bounds.LL.x = p.x - width / 2.0;
				bounds.UR.x = p.x + width / 2.0;
				break;
		}
		
		return new Text(
			new Para(
				horzAlign),
			new Char(
				para->fontsize,
				job->obj->pencolor.u.rgba[0],
				job->obj->pencolor.u.rgba[1],
				job->obj->pencolor.u.rgba[2]),
			new Run(
				bounds,
				para->str));
	}
	
	Text::Text(Para* para, Char* chars, Run* run):
		_para(para),
		_chars(chars),
		_run(run)
	{
	}
	
	Text::~Text()
	{
		if (_para)
			delete _para;
		if (_chars)
			delete _chars;
		if (_run)
			delete _run;
	}
	
	boxf Text::GetBounds() const
	{
		return _run->GetBounds();
	}
	
	void Text::Print(GVJ_t* job) const
	{
		if (_para)
			_para->Print(job);
		if (_chars)
			_chars->Print(job);
	}
	
	void Text::PrintRun(GVJ_t* job, unsigned int index) const
	{
		if (_run)
			_run->Print(job, index);
	}

	Hyperlink* Hyperlink::CreateHyperlink(GVJ_t* job, char* url, char* tooltip, char* target, char* id)
	{
		return new Hyperlink(tooltip, url, target);
	}

	Hyperlink::Hyperlink(char* description, char* address, char* frame):
		_description(agstrdup(description)),
		_address(agstrdup(address)),
		_frame(agstrdup(frame))
	{
	}
	
	Hyperlink::~Hyperlink()
	{
		agstrfree(_description);
		agstrfree(_address);
		agstrfree(_frame);
	}
	
	/* output the hyperlink */
	void Hyperlink::Print(GVJ_t* job, unsigned int id, bool isDefault) const
	{
		gvprintf(job, "<Hyperlink ID='%d'>\n", id);
		if (_description)
			gvprintf(job, "<Description>%s</Description>\n", _description);
		if (_address)
			gvprintf(job, "<Address>%s</Address>\n", _address);
		if (_frame)
			gvprintf(job, "<Frame>%s</Frame>\n", _frame);
		if (isDefault)
			gvputs(job, "<Default>1</Default>\n");
		gvputs(job, "</Hyperlink>\n");
	}
	
}