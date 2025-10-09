#include "pch.h"
#include "QuestionManager.h"
#include "ExcerciseMaker.h"
#include "FileManager.h"
#include "app_constants.h"
#include "StatMaker.h"

QuestionManager::QuestionManager()
{
    pAnswerManager = std::make_shared<AnswerManager>();
    current_question = v_excercise.cend();
}

void QuestionManager::GenerateQuestionsMixed(unsigned questions_count, unsigned clef_switch_min_notes, unsigned clef_switch_max_notes)
{
    const unsigned freq_min = std::max(1u, std::min(clef_switch_min_notes, clef_switch_max_notes));
    const unsigned freq_max = std::max(clef_switch_min_notes, clef_switch_max_notes);

    v_excercise = ExcerciseMaker()
        .SetQuestionsCount(questions_count)
        .SetMinSwitchFreqency(freq_min)
        .SetMaxSwitchFreqency(freq_max)
        .WithRepeatingNotes(false)
        .Generate();

    current_question = v_excercise.begin();
    last_question_in_pack = current_question;
    std::advance(last_question_in_pack, notes_on_screen);

    pAnswerManager->InitExcerciseSession();
}

void QuestionManager::SetOnScreenNoteCount(int note_count)
{
    if (note_count < 1 || note_count > MAX_ON_SCREEN_NOTE_COUNT)
    {
        throw std::logic_error("not supposed to happen");
    }
    notes_on_screen = note_count;
}

void QuestionManager::GoToNextQuestion()
{
    if (current_question != v_excercise.end())
        ++current_question;
    if (last_question_in_pack != v_excercise.end())
        ++last_question_in_pack;
}

void QuestionManager::GenerateQuestions(unsigned questions_count, EClefType clef)
{
    v_excercise = ExcerciseMaker()
        .SetClef(clef)
        .SetQuestionsCount(questions_count)
        .WithRepeatingNotes(false)
        .Generate();

    current_question = v_excercise.begin();
    last_question_in_pack = current_question;
    std::advance(last_question_in_pack, notes_on_screen);

    pAnswerManager->InitExcerciseSession();
}

void QuestionManager::GenerateQuestionsMixed(unsigned questions_count)
{
    v_excercise = ExcerciseMaker()
        .SetQuestionsCount(questions_count)
        .WithRepeatingNotes(false)
        .Generate();

    current_question = v_excercise.begin();
    last_question_in_pack = current_question;
    std::advance(last_question_in_pack, notes_on_screen);

    pAnswerManager->InitExcerciseSession();
}

EFileSaveResult QuestionManager::SaveExcerciseStats() const
{
    const auto res = pAnswerManager->DumpAnswerDatabaseIntoFile();
    if (res == EFileSaveResult::Success)
    {
        StatMaker::Refresh();
    }
    return res;
}

void QuestionManager::AbortExcercise()
{
    v_excercise.clear();
    current_question = v_excercise.end();
    last_question_in_pack = v_excercise.end();
    pAnswerManager->ClearSession();
}

void QuestionManager::ResetAllSessionAnswers()
{
    pAnswerManager->ResetAllCurrentSessionAnswers();
}

/* <questions, answers, excercises> */
auto QuestionManager::GetStatusText() const -> std::tuple<std::string, std::string, std::string>
{
    const auto q_str = GetQuestionsStatusText();
    const auto a_str = GetAnswersStatusText();
    const auto e_str = GetExcercisesStatusText();
    return std::make_tuple(q_str, a_str, e_str);
}

std::string QuestionManager::GetQuestionsStatusText() const
{
    if (current_question == v_excercise.cend())
    {
        return std::string();
    }
    const auto dist_end = std::distance(current_question, v_excercise.cend());
    const auto dist_begin = std::distance(v_excercise.cbegin(), current_question);
    return std::format("Question: {} / {}", dist_begin, dist_end);
}

std::string QuestionManager::GetExcercisesStatusText() const noexcept
{
    const auto total_excercises_saved = FileManager::GetSavedExcercisesCount();
    const auto unsaved_excercises = pAnswerManager->GetSessionsCount();
    return std::format("[Results] Saved: {} / Unsaved: {}", total_excercises_saved, unsaved_excercises);
}

auto QuestionManager::GetCurrentQuestionSpecificNote() const noexcept -> std::optional<ESpecificNote>
{
    if (current_question == v_excercise.cend())
    {
        return std::nullopt;
    }
    return current_question->GetSpecificNote();
}

bool QuestionManager::TakeAnswer(ENote note)
{
    const bool is_correct = IsCorrectAnswer(note);
    pAnswerManager->AddNewAnswer(current_question, is_correct);
    GoToNextQuestion();
    if (pAnswerManager->GetCount() == v_excercise.size())
    {
        if (!IsEnd())
            throw std::range_error("Iterator / Question mismatch");
        pAnswerManager->ShutdownExcerciseSession();
    }
    return is_correct;
}

auto QuestionManager::GetIterators() const noexcept -> std::pair<NoteIterator, NoteIterator>
{
    return std::make_pair(current_question, last_question_in_pack);
}
