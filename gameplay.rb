#!/usr/bin/ruby
#made by astaech

module Cupspeller
  class WordList
    @@PATH = '/home/astaech/Documents/projets/cupspeller/word_list.txt'
    @@MAX_WRD = 5

    attr_reader :word_list

    def initialize
      file = File.open @@PATH, 'r'
      list = []
      file.each_line { |line| list.push line[0..-2] }
      lines = `wc -l "#{@@PATH}"`.strip.split(' ')[0].to_i
      file.close
      @word_list = []
      @@MAX_WRD.times { @word_list.push list[rand lines] }
    end

    def to_s
      @word_list.inspect
    end

  end
end

wl = Cupspeller::WordList.new

resp = ''

time = Time.new
wl.word_list.each do |word|
  until resp == word do
    puts word
    resp = gets[0..-2]
  end
end
puts 'bravo : ' + (Time.new - time).to_s + 's'
