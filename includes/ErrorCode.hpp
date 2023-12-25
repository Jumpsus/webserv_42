#ifndef ERROR_CODE_HPP
# define ERROR_CODE_HPP

# include <stdexcept>

class ErrorCode {
    public:
        class   BadRequest : public std::exception {
            public:
                virtual const char *what() const throw(){
                    return "400";
                }
        };

        class   MovedPermanantly : public std::exception {
            public:
                virtual const char *what() const throw(){
                    return "301";
                }
        };
};

#endif