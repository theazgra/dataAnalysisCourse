class AssociationRule(object):
    def __init__(self, cells):
        self.rules = cells
        self.matchedRows = 0
        self.positiveMatchCount = 0
        self.negativeMatchCount = 0
        self.support = 0.0
        self.confidence = 0.0
    
    def ruleCount(self):
        return len(self.rules)
    
    def print_match_statistics(self):
        # Print info about property combination
        result =  self.print()
        result += "\nMatches {0} rows. Play golf: {1}x; Doesn't play: {2}x".format(self.matchedRows, self.positiveMatchCount, self.negativeMatchCount)
        result += "\nSupport: {0}; Confidence: {1}".format(self.support, self.confidence)
        result += "\n"
        return result

    def match(self, row):
        for propertyIndex in range(0, len(self.rules)):
            if not self.rules[propertyIndex].match(row[self.rules[propertyIndex].col]):
                return False
        return True

    def equals(self, other):
        if self.ruleCount() != other.ruleCount():
            return False
        else:
            for cell in self.rules:
                result = [foundCell for foundCell in other.rules if foundCell.is_same_type(cell)]
                if len(result) <= 0:
                    return False
            return True

    
    def print(self):
        result = "Combination: "
        for rule in self.rules:
            result += rule.value + ", "
        return result

    def __str__(self):
        return "Combination of {0} properties.".format(len(self.rules))    