# AI prompt history for C² Learn

## Scope and accuracy

This document records the development work that can be identified from the repository's Git history. It is a reconstruction of likely requests, not a verbatim export of prompts. The repository contains no ChatGPT/Codex conversation transcript or prompt log. The first available project commit is dated 5 November 2026, so this evidence cannot establish when AI use began or capture work before that commit. Commit summaries and changed files are the evidence for the entries below.

Exact user prompts, follow-up corrections, and assistant responses are unavailable here. The reconstructed wording is therefore intentionally phrased as task summaries. Keep this limitation visible if the document is used as an audit or coursework record.

## Reconstructed development requests

| Date | Reconstructed request (not verbatim) | Repository evidence |
| --- | --- | --- |
| 5 Nov 2026 | Set up the initial C application project and its build configuration. | `67585a1` added the initial CMake project and `main.c`. |
| 5 Nov 2026 | Expand the README with the application's purpose, features, and setup guidance. | `3969c2c` updated README and CMake configuration. |
| 5 Nov 2026 | Add a welcome screen with options to create an account, open puzzle games, or quit. | `3ba6dc3` added the welcome UI in `main.c`. |
| 9 Nov 2026 | Update the Raylib dependency to a newer version for a smoother application experience. | `20c4dd5` changed CMake and application setup. |
| 9 Nov 2026 | Add account/profile support, a lesson map, reusable UI screens, and the supporting account files. | `d014800` added account, map, screen, and UI modules. |
| 9 Nov 2026 | Implement lesson screens, profile switching, XP tracking, and progression that requires passing at least two of three questions. | `a16f8c1` added lesson questions and progression updates. |
| 10 Nov 2026 | Add puzzle games to the application. | `2c9044c` expanded `screen_puzzles.c`. |
| 11 Nov 2026 | Add more puzzles and begin a programming test feature. | `621ddff` updated lesson and puzzle screens. |
| 11 Nov 2026 | Complete puzzle features and add a pixel-art profile avatar. | `90e4ec8` added avatar modules and profile integration. |
| 13 Nov 2026 | Restyle the interface toward Codecademy, apply Jakob's usability principles, and improve typography. | `f446927` updated UI components, themes, and screens. |
| 14 Nov 2026 | Improve fonts and add a LeetCode-style coding practice section. | `80ecde4` added the editor, judge, problems, progress tracking, and practice screens. |
| 17 Nov 2026 | Add a panda helper and fix issues in the existing application. | `38f715c` added panda modules and updated app and lesson flows. |
| 17 Nov 2026 | Add industry-related learning content and features, including explore/study content and new games. | `e6c6512` added explore, study, park, 3D game, and algorithm modules. |
| 17 Nov 2026 | Add light/dark themes and lofi background music. | `11f41a6` added theme controls and focus audio. |
| 18 Nov 2026 | Add an application icon and settings mode, improve gameplay, and introduce accessibility settings and an insights screen. | `e5bbfd0` added icon, settings, accessibility, and insights changes. |
| 18 Nov 2026 | Fix chess game bugs, add interactive diagrams for System Design and AI topics, and create presentation materials. | `8230551` added chess, community hub, diagrams, and presentation files. |

## Ongoing prompt record

For future work, append each actual user request here verbatim, followed by its date and the resulting commit or files. Keep follow-up prompts as separate entries so the record distinguishes the original request from later corrections.

| Date | Exact prompt | Result / commit |
| --- | --- | --- |
| 18 Nov 2026 | “I would like to document all prompts used in the building from this application from when I commenced using you.” | Created this reconstructed history; the original historical prompt transcript is not present in the repository. |
