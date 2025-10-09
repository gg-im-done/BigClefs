#pragma once
#include "pch.h"
#include "AnswerManager.h"

using NoteIterator = std::vector<Note>::const_iterator;

class QuestionManager
{
    std::vector<Note> v_excercise;
    NoteIterator current_question;
    NoteIterator last_question_in_pack;
    std::shared_ptr<AnswerManager> pAnswerManager;
    int notes_on_screen = 6;
private:
    void GoToNextQuestion();
    [[nodiscard]] std::string GetQuestionsStatusText() const;
    [[nodiscard]] std::string GetAnswersStatusText() const noexcept { return pAnswerManager->GetAnswerStatusText(); }
    [[nodiscard]] std::string GetExcercisesStatusText() const noexcept;
    [[nodiscard]] bool IsCorrectAnswer(ENote note) const noexcept { return current_question->IsNoteEqualsTo(note); }
public:
    QuestionManager();
    void SetOnScreenNoteCount(int note_count);
    void GenerateQuestions(unsigned count, EClefType clef);
    void GenerateQuestionsMixed(unsigned count);
    void GenerateQuestionsMixed(unsigned count, unsigned clef_switch_min_notes, unsigned clef_switch_max_notes);
    void AbortExcercise();
    void ResetAllSessionAnswers();
    [[nodiscard]] EFileSaveResult SaveExcerciseStats() const;
    [[nodiscard]] auto GetAverageAnswerTime() const noexcept { return pAnswerManager->GetAverageAnswerTime(); }
    [[nodiscard]] auto GetCurrentQuestionSpecificNote() const noexcept -> std::optional<ESpecificNote>;
    [[nodiscard]] bool TakeAnswer(ENote note);
    [[nodiscard]] bool IsEnd() const noexcept { return current_question == v_excercise.end(); }
    [[nodiscard]] bool HasUnsavedExcerciseResults() const noexcept { return pAnswerManager->GetSessionsCount() != 0; }
    [[nodiscard]] auto GetCurrentClefType() const noexcept { return IsEnd() ? EClefType::Treble : current_question->GetClef(); }
    [[nodiscard]] auto GetAnswerManager() const noexcept { return pAnswerManager; }
    /* Tuple: questions, answers, excercises */
    [[nodiscard]] auto GetStatusText() const -> std::tuple<std::string, std::string, std::string>;
    [[nodiscard]] auto GetIterators() const noexcept -> std::pair<NoteIterator, NoteIterator>;
};

