# ProXPL Governance Model

The ProX Programming Language (ProXPL) project is an open-source initiative dedicated to developing a modern, high-performance programming language. This document outlines the governance structure, decision-making processes, and responsibilities of the project leadership and community.

Our goal is to foster a transparent, collaborative, and professional environment that encourages high-quality contributions while maintaining a cohesive technical vision.

## 1. Project Leadership Structure

The project is governed by a hierarchical structure designed to ensure stability and rapid development while allowing for community growth.

### 1.1. Founder and Lead Maintainer (BDFL)
The project is currently led by the Founder, who acts as the Lead Maintainer (often referred to as Benevolent Dictator for Life or BDFL).
*   **Responsibilities:**
    *   Setting the long-term strategic direction and roadmap of ProXPL.
    *   Holding the final authority on technical decisions, architectural design, and release scheduling.
    *   Acting as the tie-breaker in situations where consensus cannot be reached.
    *   Appointing and overseeing Core Maintainers.

### 1.2. Core Maintainers
Core Maintainers are trusted community members who have demonstrated a strong commitment to the project, deep technical understanding of the codebase, and alignment with the project's philosophy.
*   **Responsibilities:**
    *   Triaging issues and reviewing Pull Requests (PRs).
    *   Merging code contributions into the main repository.
    *   Guiding contributors and maintaining code quality standards.
    *   Participating in technical discussions and voting on proposals.
    *   Maintaining specific subsystems (e.g., VM, Compiler, Standard Library).

### 1.3. Contributors
Contributors are community members who submit code, documentation, or other improvements to the project.
*   **Responsibilities:**
    *   Following the `CONTRIBUTING.md` guidelines.
    *   Adhering to the `CODE_OF_CONDUCT.md`.
    *   Participating constructively in discussions.

## 2. Decision-Making Process

We strive for consensus-based decision-making. Most day-to-day decisions are made through open discussion on GitHub Issues and Pull Requests.

### 2.1. Technical Proposals (RFCs)
Substantial changes to the language, standard library, or tooling (e.g., syntax changes, new keywords, major architectural shifts) must go through a Request for Comments (RFC) process.
1.  **Drafting:** A proposal is written detailing the change, motivation, and potential impact.
2.  **Discussion:** The community discusses the proposal, offering feedback and suggestions.
3.  **Approval:**
    *   Minor changes can be approved by a Core Maintainer.
    *   Major changes require final approval from the Lead Maintainer.

### 2.2. Conflict Resolution
In cases where consensus among Core Maintainers cannot be reached:
1.  The issue enters a "cooling-off" period for further research and discussion.
2.  If the deadlock persists, the Lead Maintainer makes the final binding decision to ensure the project continues to move forward.

## 3. Contribution and Responsibility

### 3.1. Code Review Policy
*   All changes, including those from Core Maintainers, are submitted via Pull Requests.
*   Every PR requires at least one review and approval from a Core Maintainer (excluding the author) before merging.
*   For critical systems (e.g., Garbage Collector, VM Core), reviews from the Lead Maintainer or specific subject-matter experts may be mandatory.

### 3.2. Commit Access
*   Commit access (merge rights) is granted to contributors who have consistently provided high-quality contributions and demonstrated good judgment.
*   Nominations for new Core Maintainers are made by existing leadership and approved by the Lead Maintainer.

### 3.3. Code Quality
*   ProXPL prioritizes performance, readability, and stability.
*   Contributions must adhere to the style guides defined in `CODING_STANDARD.md`.
*   All tests must pass before merging.

## 4. Release and Versioning Authority

### 4.1. Versioning
ProXPL follows [Semantic Versioning (SemVer)](https://semver.org/).
*   **Major (X.y.z):** Breaking changes.
*   **Minor (x.Y.z):** New features, backward-compatible.
*   **Patch (x.y.Z):** Bug fixes, backward-compatible.

### 4.2. Release Management
*   The Lead Maintainer controls the release cycle and has the sole authority to cut official releases.
*   Release candidates may be managed by designated Release Managers under the supervision of the Lead Maintainer.

### 4.3. Breaking Changes
*   Breaking changes are avoided whenever possible.
*   When necessary, they must be rigorously justified via the RFC process and are typically reserved for major version increments.

## 5. Security and Vulnerability Handling

Security is a top priority. We follow a responsible disclosure policy.

### 5.1. Private Disclosure
*   Use the process outlined in `SECURITY.md` to report vulnerabilities.
*   **Do not** open public issues for security vulnerabilities until they have been addressed.

### 5.2. Remediation Timeline
*   The Maintainers will acknowledge receipt of a vulnerability report within 48 hours.
*   We aim to provide a fix or workaround within 90 days.
*   Once a fix is released, the vulnerability will be publicly disclosed and credited to the reporter.

## 6. Governance Evolution

ProXPL is an evolving project. As the community and codebase grow, this governance model may be updated to reflect the changing needs of the ecosystem.
*   Changes to this `GOVERNANCE.md` file follow the same decision-making process as technical changes (Discussion -> Approval).
*   Future iterations may introduce a Steering Committee or Foundation model to further decentralize leadership.

---
*This document is based on open-source best practices and adapted for the needs of the ProXPL project.*
