# Parallel_programming

IDE -  Visual Studio 2022

1. Проект Parallel_programming_ - основной проект со всеми лабораторными (за исключением тех, где на Linux системах реализуется), в нем в т.ч. запускаются дочерние процессы (.exe) в соответствующих работах.
2. Проект Process_function - его сборка необходима для получения испольняемого файла (.exe) для создания дочерних процессов в проекте Parallel_programming. Этот проект необходимо собрать первым.
3. Проект Linux_parallel_programming - сепарированный проект для реализации концепций параллельного программирования с помощью создания процессов на Unix-подобных системах. Компиляция кода осуществляется через WSL стандартной командой g++ main.cpp -o main, а запуск ./main  
