#!/usr/bin/env ruby

# Takes an clang-generated AST in stdin
# Outputs a D3-compliant JSON file

require 'json'

class CodeStruct
  attr_accessor :name, :starts_at, :size

  def initialize(parent, name, starts_at)
    @parent = parent
    @name = name
    @starts_at = starts_at
    @size = 0
    @children = []
    @parent.add_child(self) unless @parent.nil?
  end

  def ends_at(offset)
    @size = offset-@starts_at
  end

  def add_child(s)
    @children.push(s)
  end

  def as_d3_dict
    res = {name: name}
    if @children.empty?
      res[:value] = size
    else
      res[:children] = @children.map{|c| c.as_d3_dict}
    end
    res
  end

  def log(space = 0)
    puts (" "*space) + "#{name} (SIZE=#{size})"
    @children.each do |child|
      child.log(space+2)
    end
  end
end

def handleNewLine(line, index, stack)
  # Example : "    213736 |     class Timer (primary base)"
  offset, depth, name = line.match(/\s*([0-9]+)\s\|(\s+)(.*)/).captures
  offset = offset.to_i
  depth = (depth.size-1)/2

  parent = stack[depth-1]
  stack[depth..-1].each do |finished|
    finished.ends_at(offset)
  end
  stack = stack[0...depth]
  struct = CodeStruct.new(parent, name, offset)
  stack.push(struct)

  return stack
end

class_name = ARGV[0]

stack = []
ignore_line = true
STDIN.each_line.with_index do |l,i|
  if l.casecmp("         0 | class #{class_name}\n") == 0
    ignore_line = false
  end
  if l.start_with?("           | ") && ignore_line == false
    ignore_line = true
    end_offset = l.match(/sizeof=([0-9]+)/).captures.first
    end_offset = end_offset.to_i
    stack[-1].ends_at(end_offset)
    stack[0].ends_at(end_offset)
  end
  unless ignore_line
    stack = handleNewLine(l, i, stack)
  end
end

puts JSON.generate(stack[0].as_d3_dict)
