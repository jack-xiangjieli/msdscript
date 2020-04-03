//
//  pointer.hpp
//  msdscript
//
//  Created by xiangjieli on 3/3/20.
//  Copyright © 2020 xiangjieli. All rights reserved.
//

#ifndef pointer_hpp
#define pointer_hpp



#if 0

# define NEW(T) new T
# define PRT(T) T*
# define CAST(T) dynamic_cast<T*>
# define THIS this
# define ENABLE_THIS(T)

#else

# define NEW(T) std::make_shared<T>
# define PTR(T) std::shared_ptr<T>
# define CAST(T) std::dynamic_pointer_cast<T>
# define THIS shared_from_this()
# define ENABLE_THIS(T) : public std::enable_shared_from_this<T>

#endif

#endif /* pointer_hpp */
