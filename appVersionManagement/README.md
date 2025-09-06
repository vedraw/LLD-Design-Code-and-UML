Problem Src Link:
https://www.interviewstop.com/InterviewPrep/QuestionBank/design-an-app-version-management-system?category=LLD&company=PhonePe&role=Software+Engineer+2
__________________________________________________________________________________________
Design the low-level system for managing app versions across different platforms (like Android and iOS). This system will allow mobile apps to check for the latest available version, and whether an update is mandatory or optional.

🔷 Functional Requirements:
1. Add or Update App Version Info:
• Admin can configure the latest version for each platform (Android, iOS), along with:
• Version number
• Platform
• Release notes
• Update type: MANDATORY or OPTIONAL
• Minimum supported version (below which app should force logout or prevent use)

2. Check Version:
• Given a platform and current app version, return:
• Whether the app is up-to-date
• If not, whether the update is mandatory or optional
• Latest version info

3. List Version History:
• Allow admin to view historical versions, with timestamps and update types.

4. Disable a Version:
• Admin can disable certain app versions (e.g., for critical bugs), which prevents users on that version from continuing.

🔷 Constraints & Assumptions:
• Only two platforms: Android, iOS
• Version numbers follow semantic versioning: MAJOR.MINOR.PATCH
• No real-time persistence or network involved — simulate using in-memory structures
• Ignore authentication and authorization logic

🔶 Expected Design Focus:
• Classes like: VersionManager, AppVersion, VersionCheckService
• Use Enum for Platform and UpdateType
• Use a VersionComparator utility to compare semantic versions
• Follow OOP, encapsulation, and extensibility
• Design should support future platform additions (e.g., Web, Windows)

🔷 Bonus Features (Optional):
• Allow targeting a specific version to a specific region or user segment
• Add expiry date for old versions
• Store version rollout percentage (for phased rollouts)

✅ Sample Use Case:
Admin sets:
- Android → Latest: 3.2.0 (MANDATORY), Min supported: 2.5.0
- iOS → Latest: 2.1.0 (OPTIONAL), Min supported: 1.8.0

User sends version check:
- Platform: Android
- Current Version: 3.0.0

System responds:
- Status: Update Available
- Type: MANDATORY
- Latest Version: 3.2.0
