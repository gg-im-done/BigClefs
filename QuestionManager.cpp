#include "pch.h"
#include "QuestionManager.h"
#include "ExerciseMaker.h"
#include "FileManager.h"
#include "app_constants.h"
#include "StatMaker.h"

QuestionManager::QuestionManager()
{
    pAnswerManager = std::make_shared<AnswerManager>();
    current_question = v_exercise.cend();
}

void QuestionManager::GenerateQuestionsMixed(unsigned questions_count, unsigned clef_switch_min_notes, unsigned clef_switch_max_notes)
{
    const unsigned freq_min = std::max(1u, std::min(clef_switch_min_notes, clef_switch_max_notes));
    const unsigned freq_max = std::max(clef_switch_min_notes, clef_switch_max_notes);

    v_exercise = ExerciseMaker()
        .SetQuestionsCount(questions_count)
        .SetMinSwitchFreqency(freq_min)
        .SetMaxSwitchFreqency(freq_max)
        .WithRepeatingNotes(false)
        .Generate();

    current_question = v_exercise.begin();
    last_question_in_pack = current_question;
    std::advance(last_question_in_pack, notes_on_screen);

    pAnswerManager->InitExerciseSession();
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
    if (current_question != v_exercise.end())
        ++current_question;
    if (last_question_in_pack != v_exercise.end())
        ++last_question_in_pack;
}

void QuestionManager::GenerateQuestions(unsigned questions_count, EClefType clef)
{
    v_exercise = ExerciseMaker()
        .SetClef(clef)
        .SetQuestionsCount(questions_count)
        .WithRepeatingNotes(false)
        .Generate();

    current_question = v_exercise.begin();
    last_question_in_pack = current_question;
    std::advance(last_question_in_pack, notes_on_screen);

    pAnswerManager->InitExerciseSession();
}

void QuestionManager::GenerateQuestionsMixed(unsigned questions_count)
{
    v_exercise = ExerciseMaker()
        .SetQuestionsCount(questions_count)
        .WithRepeatingNotes(false)
        .Generate();

    current_question = v_exercise.begin();
    last_question_in_pack = current_question;
    std::advance(last_question_in_pack, notes_on_screen);

    pAnswerManager->InitExerciseSession();
}

EFileSaveResult QuestionManager::SaveExerciseStats() const
{
    const auto res = pAnswerManager->DumpAnswerDatabaseIntoFile();
    if (res == EFileSaveResult::Success)
    {
        StatMaker::Refresh();
    }
    return res;
}

void QuestionManager::AbortExercise()
{
    v_exercise.clear();
    current_question = v_exercise.end();
    last_question_in_pack = v_exercise.end();
    pAnswerManager->ClearSession();
}

void QuestionManager::ResetAllSessionAnswers()
{
    pAnswerManager->ResetAllCurrentSessionAnswers();
}

StatusText QuestionManager::GetStatusText() const
{
    const auto questions = GetQuestionsStatusText();
    const auto answers = GetAnswersStatusText();
    const auto exercises = GetExercisesStatusText();
    return StatusText{ questions, answers, exercises };
}

std::string QuestionManager::GetQuestionsStatusText() const
{
    if (current_question == v_exercise.cend())
    {
        return std::string();
    }
    const auto dist_end = std::distance(current_question, v_exercise.cend());
    const auto dist_begin = std::distance(v_exercise.cbegin(), current_question);
    return std::format("Question: {} / {}", dist_begin, dist_end);
}

std::string QuestionManager::GetExercisesStatusText() const noexcept
{
    const auto total_exercises_saved = FileManager::GetSavedExercisesCount();
    const auto unsaved_exercises = pAnswerManager->GetSessionsCount();
    return std::format("[Results] Saved: {} / Unsaved: {}", total_exercises_saved, unsaved_exercises);
}

auto QuestionManager::GetCurrentQuestionSpecificNote() const noexcept -> std::optional<ESpecificNote>
{
    if (current_question == v_exercise.cend())
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
    if (pAnswerManager->GetCount() == v_exercise.size())
    {
        if (!IsEnd())
            throw std::range_error("Iterator / Question mismatch");
        pAnswerManager->ShutdownExerciseSession();
    }
    return is_correct;
}

auto QuestionManager::GetIterators() const noexcept -> std::pair<NoteIterator, NoteIterator>
{
    return std::make_pair(current_question, last_question_in_pack);
}

QuestionGlyph QuestionManager::GetQuestionAtIndex(size_t index) const
{
    const size_t count = static_cast<size_t>(std::distance(current_question, last_question_in_pack));
    if (index >= count)
    {
        throw std::out_of_range("Question index out of range");
    }
    auto it = current_question;
    std::advance(it, static_cast<long long>(index));
    return QuestionGlyph{
        it->GetSpecificNote(),
        it->GetClef(),
        it->GetLedgerLine()
    };
}

size_t QuestionManager::GetOnScreenQuestionCount() const
{ 
    return static_cast<size_t>(std::distance(current_question, last_question_in_pack));
}
