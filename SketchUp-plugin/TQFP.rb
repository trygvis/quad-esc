# Copyright 2011 Trygve Laugstøl <trygvis@inamo.no>. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#   1. Redistributions of source code must retain the above copyright notice, this list of
#      conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright notice, this list
#      of conditions and the following disclaimer in the documentation and/or other materials
#      provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY J.Lamy ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL J.Lamy OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

####################################################################################
require 'sketchup.rb'
require 'extensions.rb'

class Float
    def mm
        self.to_f * 39.37
    end

    def to_mm_s
        (self.to_f / (39.37)).to_s
    end
end

class Integer
    def mm
        self.to_f * 39.37
    end

    def to_mm_s
        (self.to_f / (2.54)).to_s
    end
end

class TQFPGenerator < Sketchup::Importer

    def initialize(legCount)
        @a1 = 0.10.mm
        @a2 = 1.00.mm
        @B = 0.40.mm
        @D = 9.00.mm
        @d1 = 7.00.mm
        @d2 = (@D-@d1) / 2
        @C = 0.20.mm
        @e = 0.80.mm

        @q = 0.06.mm
        @p = 0.50.mm

        @L = 0.60.mm
        @l1 = 0.30.mm
        @l_extra = 0.05.mm

        @pin_position = 1.mm
        @pin_diameter = 0.5.mm

        @legCount = legCount

        # TODO check that d2 - L - l1 - l_extra > 0
        puts("bad? " + (@d2 - @L - @l1 - @l_extra).to_mm_s)
    end

    def newTranslation(x, y, z)
        Geom::Transformation.translation Geom::Vector3d.new(x, y, z)
    end

    def generatePackage(model)
        puts("Configuration:")
        puts("D  = " + @D.to_mm_s)
        puts("d1 = " + @d1.to_mm_s)
        puts("d2 = " + @d2.to_mm_s)
        puts("B  = " + @B.to_mm_s)
        puts("e  = " + @e.to_mm_s)
        puts("L  = " + @L.to_mm_s)
        puts("l1  = " + @l1.to_mm_s)
        puts("l_extra  = " + @l_extra.to_mm_s)

        @packageColor = 0x1f1f1f
        @legColor = 0x777777
        zero = 0.mm
        one = 1.mm
        model.entities.clear!

        package = model.entities.add_group

        # Package corners [ X, Y, Z ]
        p1 = [
            [@d2 + @q,       @d2 + @q,       @a1],
            [@d2,            @d2,            @a1 + @p],
            [@d2,            @d2,            @a1 + @p + @C],
            [@d2 + @q,       @d2 + @q,       @a1 + @p + @C + @p]]

        p2 = [
            [@d2 + @d1 - @q, @d2 + @q,                      @a1],
            [     @d2 + @d1,      @d2,                 @a1 + @p],
            [     @d2 + @d1,      @d2,            @a1 + @p + @C],
            [@d2 + @d1 - @q, @d2 + @q,       @a1 + @p + @C + @p]]

        p3 = [
            [@d2 + @d1 - @q, @d2 + @d1 - @q,                @a1],
            [     @d2 + @d1,      @d2 + @d1,           @a1 + @p],
            [     @d2 + @d1,      @d2 + @d1,      @a1 + @p + @C],
            [@d2 + @d1 - @q, @d2 + @d1 - @q, @a1 + @p + @C + @p]]

        p4 = [
            [@d2 + @q, @d2 + @d1 - @q,                      @a1],
            [     @d2,      @d2 + @d1,                 @a1 + @p],
            [     @d2,      @d2 + @d1,            @a1 + @p + @C],
            [@d2 + @q, @d2 + @d1 - @q,       @a1 + @p + @C + @p]]

        def side(entities, a, b)
            entities.add_face(a[0], b[0], b[1]).material=@packageColor
            entities.add_face(a[0], a[1], b[1]).material=@packageColor
            entities.add_line(a[0], b[1]).soft="true"

            entities.add_face(a[1], b[1], b[2]).material=@packageColor
            entities.add_face(a[1], a[2], b[2]).material=@packageColor
            entities.add_line(a[1], b[2]).soft="true"

            entities.add_face(a[2], b[2], b[3]).material=@packageColor
            entities.add_face(a[2], a[3], b[3]).material=@packageColor
            entities.add_line(a[2], b[3]).soft="true"
        end

        def cover(entities, a, b, c, d)
            entities.add_face(a, b, c).material=@packageColor
            entities.add_face(a, c, d).material=@packageColor
            entities.add_line(a, c).soft="true"
        end

        if true
            side(package.entities, p1, p2)
            side(package.entities, p2, p3)
            side(package.entities, p3, p4)
            side(package.entities, p4, p1)
            cover(package.entities, p1[0], p2[0], p3[0], p4[0])
            cover(package.entities, p1[3], p2[3], p3[3], p4[3])

            # Generating the pin 1 identifier doesn't quite work yet
            vector = Geom::Vector3d.new(0, 0, 1)
            vector = vector.normalize!

            # Place the pin in the middle of a face instead of, just to make the pushpull() stuff a bit easier to implement.
            # As the pin ends up in the bottom right cornder (seen from the top), the model has to be rotated 90 degrees later
            pin_edges = package.entities.add_circle(Geom::Point3d.new(@D - @d2 - @q - @pin_position, @d2 + @q + @pin_position, p1[3].z), vector, @pin_diameter)
            pin_edges[0].faces[0].pushpull(-0.2.mm, false)
#            pin_edges[12].faces[0].pushpull(-0.2.mm, false)

#            pin_edges = package.entities.add_circle(Geom::Point3d.new(@D/2, @D/2, p1[3].z), Geom::Vector3d.new(0, 0, 1), 0.5.mm)
#            package.entities.add_face(pin_edges).pushpull(0.2.mm, false)
        end

        def quad(entities, vertices, material)
            entities.add_face(vertices[0], vertices[1], vertices[2]).material=material
            entities.add_face(vertices[0], vertices[2], vertices[3]).material=material
            entities.add_line(vertices[0], vertices[2]).soft="true"
        end

        # The leg is constructed from the underside and up
        # starting with the part the furthest away from the package
        # The start point is 0,0,0
        # TODO: Consider making this a component instead. It would make
        # post-processing easier as it would be possible to change all
        # components later in the SketchUp GUI or through a script.
        def leg(e)
            group = e.add_group
            entities = group.entities
            t_c = newTranslation 0, 0, @C
            t_b = newTranslation @B, 0, 0
            t_l_extra_neg = newTranslation 0, -@l_extra, 0

            # L segment's plane
            l_lower = [
                Geom::Point3d.new([0,   0,      0]),
                Geom::Point3d.new([@B,  0,      0]),
                Geom::Point3d.new([@B, @L, 0]),
                Geom::Point3d.new([0,  @L, 0]),
            ]
            # The upper plane is @l_extra shorter
            l_upper = [
                t_c * l_lower[0],
                t_c * l_lower[1],
                t_l_extra_neg * t_c * l_lower[2],
                t_l_extra_neg * t_c * l_lower[3],
            ]

            l_left_panel = [
                l_lower[0], 
                l_upper[0],
                l_upper[3],
                l_lower[3],
            ]
            l_right_panel = l_left_panel.map {|v| t_b * v}

            l_end_panel = [
                l_lower[0], 
                l_right_panel[0],
                l_right_panel[1],
                l_upper[0],
            ]

            l1_lower = [
                Geom::Point3d.new([ 0, @d2 - @l1, @a1 + @p]),
                Geom::Point3d.new([@B, @d2 - @l1, @a1 + @p]),
                Geom::Point3d.new([@B, @d2, @a1 + @p]),
                Geom::Point3d.new([ 0, @d2, @a1 + @p]),
            ]
            # The upper plane is @l_extra longer
            l1_upper = [
                t_l_extra_neg * t_c * l1_lower[0],
                t_l_extra_neg * t_c * l1_lower[1],
                t_c * l1_lower[2],
                t_c * l1_lower[3],
            ]

            l1_left = [
                l1_lower[0], 
                l1_upper[0],
                l1_upper[3],
                l1_lower[3],
            ]
            l1_right = l1_left.map {|v| t_b * v}

            l2_lower = [
                l_lower[3],
                l1_lower[0],
                l1_lower[1],
                l_lower[2],
            ]

            l2_upper = [
                l_upper[3],
                l1_upper[0],
                l1_upper[1],
                l_upper[2],
            ]

            l2_left = [
                l_lower[3],
                l_upper[3],
                l1_upper[0],
                l1_lower[0],
            ]
            l2_right = l2_left.map {|v| t_b * v}

            quad(entities, l_lower, @legColor)
            quad(entities, l_upper, @legColor)

            quad(entities, l_left_panel, @legColor)
            quad(entities, l_right_panel, @legColor)

            quad(entities, l_end_panel, @legColor)

            quad(entities, l1_lower, @legColor)
            quad(entities, l1_upper, @legColor)
            quad(entities, l1_left, @legColor)
            quad(entities, l1_right, @legColor)

            quad(entities, l2_lower, @legColor)
            quad(entities, l2_upper, @legColor)
            quad(entities, l2_left, @legColor)
            quad(entities, l2_right, @legColor)

            group
        end

        def legs(entities)
            group = entities.add_group
            legSpacing = @e
            leg = leg(group.entities)
            (0 .. @legCount - 1).each do |i|
                leg.transformation = newTranslation @d2 + (@e / 2) + i * @e, 0, 0
                leg = leg.copy
            end
            group
        end

        if true
            legs = legs(package.entities).copy
            legs.transformation = 
                (Geom::Transformation.rotation Geom::Point3d.new(0, 0, 0), Geom::Vector3d.new(0, 0, 1), Math::PI / 2) *
                (newTranslation 0, -@D, 0)
            legs = legs.copy
            legs.transformation = 
                (Geom::Transformation.rotation Geom::Point3d.new(0, 0, 0), Geom::Vector3d.new(0, 0, 1), Math::PI) *
                (newTranslation -@D, -@D, 0)
            legs = legs.copy
            legs.transformation = 
                (Geom::Transformation.rotation Geom::Point3d.new(0, 0, 0), Geom::Vector3d.new(0, 0, 1), Math::PI * 1.5) *
                (newTranslation -@D, 0, 0)
        end

        # Move and rotate the package into it's final position
        package.transformation = 
            (newTranslation @D / 2, @D / 2, 0) * 
            (Geom::Transformation.rotation Geom::Point3d.new(0, 0, 0), Geom::Vector3d.new(0, 0, 1), Math::PI)
    end
end

def onGenerate(tqfpGenerator)
    sketchup = Sketchup.file_new
    model = sketchup.active_model
    tqfpGenerator.generatePackage(model)
end

tqfpGenerator = TQFPGenerator.new(8)

UI.menu("PlugIns").add_item("Generate TQFP package") {
    onGenerate(tqfpGenerator)
}

 onGenerate(tqfpGenerator)
