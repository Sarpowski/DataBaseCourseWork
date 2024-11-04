# DataBaseCourseWork
Automated Dean's Office System


Automated Dean's Office System

This project is a part of a series of lab assignments aimed at automating the work of the dean's office using PostgreSQL and a C++ client application built with Qt. The project involves creating and managing database objects to store and analyze student performance data in a structured way.
Project Overview

The purpose of this project is to automate the process of managing student grade statistics for a dean's office. The automation does not involve analyzing the time constraints for course completions or the prerequisites for subjects. The application focuses on tracking and analyzing the performance of students in various subjects assigned by professors.
Database Structure

The PostgreSQL database will contain the following objects:

    Students: Characterized by last name, first name, and middle name.
    Professors: Characterized by last name, first name, and middle name.
    Subjects: Characterized by a name.
    Grades: Represented as an integer in the range [2..5]. Each grade is associated with a student, a subject, and the professor who assigned the grade.
    Groups: Each student belongs to only one group. A group is characterized by a name formatted as <course>/<direction>/<subgroup>_<year>, e.g., 3084/1_2004.

Key Features

    Data Integrity and Relationships:
        Students are linked to groups.
        Grades are linked to students, subjects, and professors.
        Foreign key constraints ensure data consistency.
    Performance Analysis:
        Analyze student performance by groups, subjects, professors, and individual students.
        Generate year-based performance trends by averaging grades across the academic year, extracted from the group name.
