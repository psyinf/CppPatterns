#include <atomic>
#include <future>
#include <memory>
#include <numeric>
#include <iostream>
#include <vector>
#include <chrono>
/**
 * \brief	A generator strategy interface.
 * \date	01.10.2018
 * \tparam	ElementType	Type of the element type.
 */

template <typename ElementType = uint32_t> 
class IGeneratorStrategy
{
public:
	using Elements = std::vector<ElementType>;

	virtual auto generate(size_t count, ElementType start = ElementType()) ->Elements = 0;
};

/**
 * \brief	A summation strategy interface.
 * \date	01.10.2018
 * \tparam	ElementType	Type of the element type.
 */

template <typename ElementType> 
class ISummationStrategy
{
public:
	using Elements = std::vector<ElementType>;

	virtual auto sum(const Elements& elements) ->ElementType = 0;
};


template <typename ElementType>
class LinearSummationStrategy : public ISummationStrategy<ElementType>
{
	using Elements = std::vector<ElementType>;
public:
	virtual auto sum(const Elements& elements) -> ElementType override
	{
		return std::accumulate(std::cbegin(elements), std::cend(elements), ElementType());
	}

};
template <typename ElementType>
class ParallelSummationStrategy : public ISummationStrategy<ElementType>
{
	using Elements = std::vector<ElementType>;
public:
	virtual auto sum(const Elements& elements) -> ElementType override
	{
		using elem_iter = typename Elements::const_iterator;
		std::function<ElementType(elem_iter,elem_iter)> parallel_sum;
		
		parallel_sum = [&parallel_sum,size = elements.size()](auto beg, auto end)->ElementType
		{
			auto len = end - beg;
			if (len <= size / 25) //reasonable amout  of threads
			{
				return std::accumulate(beg, end, ElementType(0));
			}
			auto mid = beg + len / 2;
			auto handle = std::async(std::launch::async, parallel_sum, mid, end);

			int sum = parallel_sum(beg, mid);
			return handle.get() + sum ;
		};

		return parallel_sum(std::cbegin(elements), std::cend(elements));
	}

};



template <typename ElementType> 
class SequenceGeneratorStrategy : public IGeneratorStrategy<ElementType>
{

	using Elements = std::vector<ElementType>;
public:
	virtual auto generate(size_t count, ElementType start = ElementType()) -> Elements override
	{
	 	Elements return_values(count);
		std::iota(std::begin(return_values), std::end(return_values), start);
		return return_values;
	}

};

template <typename ElementType> 
class PrimeSequenceGenerator : public IGeneratorStrategy<ElementType>
{

	using Elements = std::vector<ElementType>;
public:
	virtual auto generate(size_t count, ElementType start /* = ElementType() */) -> Elements
	{
		//Sieve of Eratosthenes
		Elements primes;
		std::vector<bool> stroke(count + 1, true);
		
		for (auto p = 2; p * p < count; ++p)
		{
			// If prime[p] is not changed, then it is a prime 
			if (stroke[p])
			{
				// Update all multiples of p 
				for (auto i = p * 2; i <= count; i += p)
				{
					stroke[i] = false;
				}
					
			}
		}
		auto distance = 0;
		for (auto prime : stroke)
		{
			if (prime)
			{
				primes.push_back(distance);
			}
			++distance;
			
		}
		return primes;
	}
};

	

/**
 * \brief	A sum generator using the Strategy pattern to .
 * \date	01.10.2018
 * \tparam	ElementType	Type of the element type.
 */

template <typename ElementType>
class SumMaker
{
	using Generator = IGeneratorStrategy<ElementType>;
	using Summator = ISummationStrategy<ElementType>;

public:
	SumMaker(std::unique_ptr<Generator>&& generator, std::unique_ptr<Summator>&& summator)
		:mGenerator(std::move(generator))
		,mSummator(std::move(summator))
	{
	}

	auto make(size_t count, ElementType start ) -> ElementType
	{
		return mSummator->sum(mGenerator->generate(count, start));
	}
protected:
	std::unique_ptr<Generator> mGenerator;
	std::unique_ptr<Summator> mSummator;

};


int main(void)
{
	auto generator_strategy = std::make_unique<SequenceGeneratorStrategy<uint32_t>>();
	auto prime_generator_strategy = std::make_unique<PrimeSequenceGenerator<uint32_t>>();
	auto summator_strategy = std::make_unique<LinearSummationStrategy<uint32_t>>();
	auto par_summator_strategy = std::make_unique<ParallelSummationStrategy<uint32_t>>();
	//SumMaker<uint32_t>maker(std::move(generator_strategy), std::move(summator_strategy));
	
	using clock = std::chrono::high_resolution_clock;
	size_t count = 10000000;
	{
		auto start = clock::now();
		
		SumMaker<uint32_t> prime_maker(std::make_unique<SequenceGeneratorStrategy<uint32_t>>(), std::move(summator_strategy));
		std::cout << "Sum is " << prime_maker.make(count, 1) << std::endl;
		std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start).count() << " ms\n";
	}
	
	{
		auto start = clock::now();
		SumMaker<uint32_t> prime_maker(std::make_unique<SequenceGeneratorStrategy<uint32_t>>(), std::move(par_summator_strategy));
		std::cout << "Sum is " << prime_maker.make(count, 1) << std::endl;
		std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start).count() << " ms\n";
	}
	return 0;	
}
