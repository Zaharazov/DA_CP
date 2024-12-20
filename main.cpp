#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

std::vector<int> dp(const std::vector<std::string> & words1, const std::vector<std::string> & words2)
{
    int len1 = words1.size(), len2 = words2.size();
    std::vector<int> dp(len2 + 1, 0);
    
    for (int i = 1; i < len1 + 1; ++i) 
    {
        int diag = 0;
        for (int j = 1; j < len2 + 1; ++j) 
        {
            int pred_dp = dp[j];
            if (words1[i - 1] == words2[j - 1])
            {
                dp[j] = diag + 1;
            }
            else
            {
                dp[j] = std::max(dp[j - 1], dp[j]);
            }
            diag = pred_dp;
        }
    }
    return dp;
}

std::vector<std::string> LCS(const std::vector<std::string> & words1, const std::vector<std::string> & words2, std::vector<int> & left_words1, std::vector<int> & right_words1)
{
    int len1 = words1.size(), len2 = words2.size();
    
    // базовые случаи
    
    if (len1 == 0 or len2 == 0) return {};
    
    if (len1 == 1)
    {
        if (std::find(words2.begin(), words2.end(), words1[0]) != words2.end()) return {words1[0]};
        return {};
    }
    
    if (len2 == 1)
    {
        if (std::find(words1.begin(), words1.end(), words2[0]) != words1.end()) return {words2[0]};
        return {};
    }
    
    // чуть чуть динамики
    
    int middle1 = len1 / 2;
    
    std::vector<std::string> reverse1 = std::vector<std::string>(words1.begin() + middle1, words1.end());
    std::vector<std::string> reverse2 = words2;
    
    std::reverse(reverse1.begin(), reverse1.end());
    std::reverse(reverse2.begin(), reverse2.end());
    
    left_words1 = dp(std::vector<std::string>(words1.begin(), words1.begin() + middle1), words2);
    right_words1 = dp(reverse1, reverse2);
    
    int middle2 = 0;
    for (int i = 0; i <= len2; ++i)
    {
        if (left_words1[i] + right_words1[len2 - i] > left_words1[middle2] + right_words1[len2 - middle2])  
            middle2 = i;
    }
    
    std::vector<std::string> otvet;
    std::vector<std::string> left_LCS = LCS(std::vector<std::string>(words1.begin(), words1.begin() + middle1), std::vector<std::string>(words2.begin(), words2.begin() + middle2), left_words1, right_words1);
    std::vector<std::string> right_LCS = LCS(std::vector<std::string>(words1.begin() + middle1, words1.end()), std::vector<std::string>(words2.begin() + middle2, words2.end()), left_words1, right_words1);

    
    otvet.insert(otvet.end(), left_LCS.begin(), left_LCS.end());
    otvet.insert(otvet.end(), right_LCS.begin(), right_LCS.end());
    
    return otvet;
}

int main()
{
    // ввод данных для обработки
    
    std::string text1, text2;
    
    std::getline(std::cin, text1);
    std::getline(std::cin, text2);
    
    std::string word;
    std::vector<std::string> words1, words2;
    
    std::istringstream stream1(text1);
    std::istringstream stream2(text2);
    
    while (stream1 >> word) {
        words1.push_back(word); 
    }

    while (stream2 >> word) {
        words2.push_back(word);
    }
    
    // запускаем алгоритм Хиршберга
    
    std::vector<int> left_words1;
    std::vector<int> right_words1;
    
    std::vector<std::string> otvet = LCS(words1, words2, left_words1, right_words1);
    
    // выводим ответ
    
    std::cout << otvet.size() << std::endl;
    for (int i = 0; i < otvet.size(); ++i)
    {
        std::cout << otvet[i] << " ";
    }
    
    return 0;
}
