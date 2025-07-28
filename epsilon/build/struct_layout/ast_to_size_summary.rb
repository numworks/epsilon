#!/usr/bin/env ruby

# Takes an clang-generated AST in stdin
# Outputs a csv summary of some class sizes

#require_relative 'ast_parser'

class_names = ARGV

class_of_interest = nil
puts ("CLASS NAMES,SIZES")
puts
STDIN.each_line.with_index do |l,i|
  for class_name in class_names do
    if l.casecmp("         0 | class #{class_name}\n") == 0
      class_of_interest = class_name
    end
  end
  if l.start_with?("           | [sizeof") && !class_of_interest.nil?
    size = l.match(/sizeof=([0-9]+)/).captures.first
    size = size.to_i
    puts ("#{class_of_interest},#{size}")
    class_of_interest = nil
  end
end
