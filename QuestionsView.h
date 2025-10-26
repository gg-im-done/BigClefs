#pragma once
#include "pch.h"
#include "cute_enums.h"

struct QuestionGlyph
{
    ESpecificNote specific_note;
    EClefType clef;
    int ledger_line_count;
};

struct IQuestionsView
{
    virtual ~IQuestionsView() = default;
    virtual EClefType GetCurrentClef() const = 0;
    virtual size_t GetOnScreenQuestionCount() const = 0;
    virtual QuestionGlyph GetQuestionAtIndex(size_t index) const = 0;
};
