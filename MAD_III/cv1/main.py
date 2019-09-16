from itertools import combinations, chain


class Pattern:
    def __init__(self, items):
        self.items = items

    def __str__(self):
        return "Pattern items: {}".format(self.items)


def powerset_set(iterable):
    s = list(iterable)
    return set(chain.from_iterable(combinations(s, r) for r in range(len(s)+1)))


def occurence_count(transactions, pattern):
    result = 0
    for t in transactions:
        if (transactions & pattern) == pattern:
            result += 1
    return result


def main():
    # 1.
    indices = range(0, 6)
    indices_combinations = combinations(indices, 3)
    print(list(indices_combinations))

    # 2.
    data_file_name = "cv1/data/chess.dat"
    transactions = []
    with open(data_file_name, 'r') as data_file:
        for line in data_file:
            transactions.append(list(map(int, line.strip().split(' '))))

    transaction_count = len(transactions)
    items = []
    for t in transactions:
        items += t

    items = sorted(set(items))
    item_count = len(items)
    print("Item count in {}: {}".format(data_file_name, item_count))

    min_support = 0.950
    strong_patterns = []
    for pattern_size in range(1, (item_count + 1)):
        generated_patterns = map(set, combinations(items, pattern_size))
        current_patterns = []

        for pattern in generated_patterns:
            all_subsets_are_strong = True
            if (pattern_size > 1):
                pattern_subsets = powerset_set(pattern)
                for subset in pattern_subsets:
                    if subset == {}:
                        continue
                    else:
                        subset_size = len(subset)
                        if subset not in strong_patterns[subset_size - 1]:
                            all_subsets_are_strong = False
                            break
            if not all_subsets_are_strong:
                continue
            pattern_match_count = occurence_count(transactions, pattern)
            pattern_support = float(pattern_match_count) / float(transaction_count)
            if (pattern_support >= min_support):
                current_patterns.append(pattern)

        strong_patterns.append(current_patterns)

        # pattern_indices = combinations(unique_items, pattern_size)
        # print("pattern of size {} = {}".format(pattern_size, list(pattern_indices)))


if __name__ == "__main__":
    main()
