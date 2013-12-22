#include "spec_helper.h"
#include <memory>

using namespace tree_sitter::lr;
using namespace tree_sitter::rules;

static item_set_ptr item_set(const std::initializer_list<Item> &items) {
    return item_set_ptr(new ItemSet(items));
}

Describe(item_sets) {
    Grammar grammar = test_grammars::arithmetic();
    
    It(computes_the_closure_of_an_item_set_under_symbol_expansion) {
        Item item = Item::at_beginning_of_rule("expression", grammar);
        ItemSet set = ItemSet(item, grammar);
        
        AssertThat(
            set,
            EqualsContainer(ItemSet({
                Item("expression", grammar.rule("expression"), 0),
                Item("term", grammar.rule("term"), 0),
                Item("factor", grammar.rule("factor"), 0),
                Item("variable", grammar.rule("variable"), 0),
                Item("number", grammar.rule("number"), 0),
                Item("left_paren", grammar.rule("left_paren"), 0),
            })));
    }
    
    It(computes_transitions) {
        Item item = Item::at_beginning_of_rule("factor", grammar);
        ItemSet set = ItemSet(item, grammar);
        
        AssertThat(
            set.sym_transitions(grammar),
            Equals(transition_map<rules::Symbol, ItemSet>({
                { sym("variable"), item_set({ Item("factor", blank(), 1) }) },
                { sym("number"), item_set({ Item("factor", blank(), 1) }) },
                { sym("left_paren"), std::make_shared<ItemSet>(Item("factor", seq({ sym("expression"), sym("right_paren") }), 1), grammar) },
            })));
    }
    
    It(computes_character_transitions) {
        Item item = Item::at_beginning_of_rule("factor", grammar);
        ItemSet set = ItemSet(item, grammar);

        AssertThat(
            set.char_transitions(grammar),
            Equals(transition_map<rules::Character, ItemSet>({
                { character(CharClassWord), item_set({ Item("variable", choice({ repeat(character(CharClassWord)), blank() }), 1) }) },
                { character(CharClassDigit), item_set({ Item("number", choice({ repeat(character(CharClassDigit)), blank() }), 1) }) },
                { character('('), item_set({ Item("left_paren", blank(), 1) }) }
            })));
    }
    
    It(can_be_hashed) {
        ItemSet set1 = ItemSet(Item::at_beginning_of_rule("factor", grammar), grammar);
        ItemSet set2 = ItemSet(Item::at_beginning_of_rule("factor", grammar), grammar);
        AssertThat(std::hash<const ItemSet>()(set1), Equals(std::hash<const ItemSet>()(set2)));

        ItemSet set3 = ItemSet(Item::at_beginning_of_rule("term", grammar), grammar);
        AssertThat(std::hash<const ItemSet>()(set1), !Equals(std::hash<const ItemSet>()(set3)));
    }
};
